/*-
 This file is part of AFF4 CPP.

 AFF4 CPP is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 AFF4 CPP is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with AFF4 CPP.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "aff4config.h"
#include "aff4.h"
#include "aff4-c.h"

#include <cerrno>
#include <map>
#include <memory>

/**
 * Next handle.
 */
static int nextHandle;

/**
 * Package version string
 */
static std::string version(PACKAGE_STRING);

/**
 * Typedef for held container references.
 * <p>
 * We have a resolver, the parent container, and the aff4:Stream for the map.s
 */
typedef typename std::tuple<std::shared_ptr<aff4::IAFF4Resolver>, //
		std::shared_ptr<aff4::IAFF4Container>, //
		std::shared_ptr<aff4::IAFF4Stream>> container_t;

/**
 * Map of open containers.
 */
static std::shared_ptr<std::map<int, container_t>> handles = nullptr;

extern "C" {

/**
 * Get the AFF4 version
 */
const char* AFF4_version() {
	return version.c_str();
}

/**
 * Initialise libaff4.
 */
void AFF4_init() {
	nextHandle = 1;
	handles = std::make_shared<std::map<int, container_t>>();
}

int AFF4_open(const char* filename) {
	if (handles == nullptr) {
		AFF4_init();
	}
	std::string file(filename);
	if (file.empty()) {
		errno = ENOENT;
		return -1;
	}
	// Attempt to open the file.
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file);
	if (container == nullptr) {
		errno = ENOENT;
		return -1;
	}

	// Attempt to start a resolver
	std::shared_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(file));
	if (resolver == nullptr) {
		errno = ENOENT;
		return -1;
	}
	container->setResolver(resolver.get());

	// Open the first image.
	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = container->getImages();
	if (images.empty()) {
		errno = ENOENT;
		return -1;
	}
	if (images[0] == nullptr) {
		errno = ENOENT;
		return -1;
	}
	// Get the map for the first image.
	std::shared_ptr<aff4::IAFF4Map> map = images[0]->getMap();
	if (map == nullptr) {
		errno = ENOENT;
		return -1;
	}
	std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
	if (stream == nullptr) {
		errno = ENOENT;
		return -1;
	}
	container_t handleEntry = std::make_tuple(resolver, container, stream);
	int handle = nextHandle;
	(*handles)[handle] = handleEntry;
	nextHandle++;
	return handle;
}

int64_t AFF4_object_size(int handle) {
	if (handles == nullptr) {
		AFF4_init();
	}
	auto it = handles->find(handle);
	if (it != handles->end()) {
		container_t con = it->second;
		return std::get<2>(con)->size();
	}
	errno = EBADF;
	return -1;
}

int AFF4_read(int handle, uint64_t offset, void* buffer, int length) {
	if (handles == nullptr) {
		AFF4_init();
	}
	if (buffer == nullptr) {
		errno = EFAULT;
		return -1;
	}
	if (length <= 0) {
		errno = EINVAL;
		return -1;
	}
	auto it = handles->find(handle);
	if (it != handles->end()) {
		container_t con = it->second;
		std::shared_ptr<aff4::IAFF4Stream> stream = std::get<2>(con);
		if (offset > stream->size()) {
			errno = EINVAL;
			return -1;
		}
		return (int) stream->read(buffer, length, offset);
	}
	errno = EBADF;
	return -1;
}

int AFF4_close(int handle) {
	if (handles == nullptr) {
		AFF4_init();
	}
	auto it = handles->find(handle);
	if (it != handles->end()) {
		container_t con = it->second;
		// Close the container
		std::get<1>(con)->close();
		// And remove the map entry.
		handles->erase(it);
		return 0;
	}
	errno = EBADF;
	return -1;
}

}
