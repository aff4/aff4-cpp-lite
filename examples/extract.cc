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

/*
 * Example application to create a SHA1 digest of all images in the container.
 */

#include "aff4.h"
#include "utils.h"
#include <cstdio>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <fcntl.h>
#include <algorithm>
/**
 * Print usage
 * @param argv
 */
void printUsage(char* argv[]) {
	std::cout << "USAGE: " << argv[0] << " <input filename> <output filename>" << std::endl;
}

/**
 * Main
 * @param argc number of arguments
 * @param argv args
 * @return EXIT_SUCCESS
 */
int main(int argc, char* argv[]) {
	std::cout << "AFF4 Container Information (" << aff4::getVersionString() << ")" << std::endl;
	if (argc < 3) {
		printUsage(argv);
		return 1;
	}
	std::string filename(argv[1]);
	if (filename.empty()) {
		printUsage(argv);
		return 1;
	}

	std::string outputfilename(argv[2]);
	if (outputfilename.empty()) {
		printUsage(argv);
		return 1;
	}

	/*
	 * Create a resolver to allow the container to find any external resources.
	 */

	std::unique_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(filename));
	if (resolver == nullptr) {
		std::cout << "Unable to construct a resolver?" << std::endl;
	}

	/*
	 * Open the container
	 */

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename, resolver.get());
	if (container == nullptr) {
		std::cout << "Unable to open the file \"" << filename << "\"?" << std::endl;
	}

	/*
	 * Print the container's resource values.
	 */

	std::cout << "Container : " << container->getResourceID() << std::endl;
	std::cout << " Images: " << std::endl;

	/*
	 * Get a vector of all aff4:Image instances in this container
	 */

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = container->getImages();
	for (std::shared_ptr<aff4::IAFF4Image> image : images) {

		/*
		 * Print the image's resource values.
		 */

		std::cout << "  Image : " << image->getResourceID() << std::endl;
		printProperties(image, "    ");

		/*
		 * Get the map instance of the image, and create a digest of the primary stream.
		 */

		uint64_t count = 0;
		uint64_t countMarker = 128 * 1024 * 1024;

		std::shared_ptr<aff4::IAFF4Map> map = image->getMap();
		if (map != nullptr) {
			std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
			if (stream != nullptr) {
				uint64_t length = stream->size();
				uint64_t remaining = length;
				uint64_t offset = 0;
				uint64_t bufferSize = 1024 * 1024; // default 1MB buffer.

				/*
				 * Allocate buffer to read to
				 */
				std::unique_ptr<uint8_t[]> buffer(new uint8_t[bufferSize]);
				if (buffer.get() == nullptr) {
					std::cout << "Unable to allocate buffer?" << std::endl;
					return 1;
				}

				/*
				 *  Open output stream.
				 */

				FILE* fileHandle = ::fopen(outputfilename.c_str(), "w+");
				if (fileHandle == nullptr) {
					std::cout << "Unable to open output stream?" << std::endl;
					return 1;
				}
				std::cout << "Writing to " << outputfilename << " ";

				while (remaining > 0) {
					/*
					 * read and write buffer fulls at a time
					 */
					uint64_t sz = std::min(bufferSize, remaining);
					int64_t res = stream->read(buffer.get(), sz, offset);
					if (res > 0) {
						::fwrite(buffer.get(), res, 1, fileHandle);
						remaining -= res;
					} else {
						// read failed?
						remaining = 0;
					}
					offset += res;

					count += res;

					count += res;
					if (count > countMarker) {
						std::cout << ".";
						std::cout.flush();
						count = 0;
					}
				}
				::fclose(fileHandle);
				std::cout << std::endl;

			} else {
				std::cout << "Unable to open stream?" << std::endl;
				return 1;
			}
		} else {
			std::cout << "Unable to open map?" << std::endl;
			return 1;
		}

	}
	std::cout << std::endl;
}
