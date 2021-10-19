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

#include "LightResolver.h"
#include "StringUtil.h"

namespace aff4 {
namespace resolver {

LightResolver::LightResolver(const std::string& resource, const std::string& path, bool scanSubFolders) :
		AFF4Resource(resource), basePath(path), scanSubFolders(scanSubFolders) {

	// Single property.
	addProperty(aff4::Lexicon::AFF4_FILENAME, aff4::rdf::RDFValue(path));
#if DEBUG_VERBOSE
	fprintf( aff4::getDebugOutput(), "%s[%d] : LightResolver: %s\n", __FILE__, __LINE__, resource.c_str());
#endif
	// refresh
	std::lock_guard<std::recursive_mutex> lock(mapLock);
	scanForAFF4Volumes(basePath);
}

LightResolver::~LightResolver() {
	// NOP
}

/*
 * From IAFF4Resolver
 */
std::shared_ptr<aff4::IAFF4Resource> LightResolver::open(const std::string& resource) noexcept {
#if DEBUG_VERBOSE
	fprintf( aff4::getDebugOutput(), "%s[%d] : Open Resource %s\n", __FILE__, __LINE__, resource.c_str());
#endif
	if (hasResource(resource)) {
		// This will unlock when it goes out of scope.
		std::lock_guard<std::recursive_mutex> lock(mapLock);
		std::string filename = volumes[resource];
		if (aff4::util::fileExists(filename)) {
			return aff4::container::openAFF4Container(filename, this);
		} else {
			// remove entry from map.
			volumes.erase(resource);
		}
	}
	return nullptr;
}

bool LightResolver::hasResource(const std::string& resource) noexcept {
#if DEBUG_VERBOSE
	fprintf( aff4::getDebugOutput(), "%s[%d] : Has Resource %s\n", __FILE__, __LINE__, resource.c_str());
#endif
	if (resource.empty()) {
		return false;
	}
	// This will unlock when it goes out of scope.
	std::lock_guard<std::recursive_mutex> lock(mapLock);
	if (volumes.find(resource) != volumes.end()) {
		return true;
	}
	// Rescan for new files.
	scanForAFF4Volumes(basePath);
	if (volumes.find(resource) != volumes.end()) {
		return true;
	}
	return false;
}

bool LightResolver::mapContains(const std::string& filename) {
	for (auto it = volumes.begin(); it != volumes.end(); it++) {
		if (it->second.compare(filename) == 0) {
			return true;
		}
	}
	return false;
}

void LightResolver::scanForAFF4Volumes(const std::string& path) {
	// It is expected that the map is LOCKED prior to this call.
	if (path.empty()) {
		return;
	}
#if DEBUG_VERBOSE
	fprintf( aff4::getDebugOutput(), "%s[%d] : Scanning Path %s\n", __FILE__, __LINE__, path.c_str());
#endif
#ifdef _WIN32
	/*
	* Windows based systems
	*/
	std::wstring wpath = aff4::util::s2ws(path);

	std::wstring pattern(wpath);
	pattern.append(L"\\*");

	WIN32_FIND_DATAW data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
	    do {
			std::wstring filename(data.cFileName);
			if (filename.compare(L".") && filename.compare(L"..")) {
				std::string absoluteFilename = path + "\\" + aff4::util::ws2s(filename);
				if ((aff4::util::isFile(absoluteFilename)) && (aff4::container::isAFF4Container(aff4::util::ws2s(filename)))) {
					if (!mapContains(absoluteFilename)) {
						// We don't have this file
						std::string resID = aff4::container::getResourceID(absoluteFilename);
						if (!resID.empty()) {
#if DEBUG_VERBOSE
							fprintf(aff4::getDebugOutput(), "%s[%d] : Adding Volume %s => %s \n", __FILE__, __LINE__, resID.c_str(), absoluteFilename.c_str());
#endif
							volumes[resID] = absoluteFilename;
						}
					}
				}
				else if (aff4::util::isDirectory(absoluteFilename)) {
					if (scanSubFolders) {
						scanForAFF4Volumes(absoluteFilename);
					}
				}
			}
	    } while (FindNextFile(hFind, &data) != 0);
	    FindClose(hFind);
	}
#else
	/*
	* POSIX based systems.
	*/
	DIR* dirp = ::opendir(path.c_str());
	struct dirent * dp;
	while ((dp = readdir(dirp)) != NULL) {
		std::string filename(dp->d_name);
		if (filename.compare(".") && filename.compare("..")) {
			std::string absoluteFilename = path + "/" + filename;
			if ((aff4::util::isFile(absoluteFilename)) && (aff4::container::isAFF4Container(filename))) {
				if (!mapContains(absoluteFilename)) {
					// We don't have this file
					std::string resID = aff4::container::getResourceID(absoluteFilename);
					if (!resID.empty()) {
#if DEBUG_VERBOSE
						fprintf(aff4::getDebugOutput(), "%s[%d] : Adding Volume %s => %s \n", __FILE__, __LINE__, resID.c_str(), absoluteFilename.c_str());
#endif
						volumes[resID] = absoluteFilename;
					}
				}
			} else if (aff4::util::isDirectory(absoluteFilename)) {
				if (scanSubFolders) {
					scanForAFF4Volumes(absoluteFilename);
				}
			}
		}
	}
	closedir(dirp);
#endif
}

/*
 * AFF4 Resource
 */

std::string LightResolver::getResourceID() const noexcept {
	return AFF4Resource::getResourceID();
}

std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> LightResolver::getProperties() noexcept {
	return AFF4Resource::getProperties();
}

std::vector<aff4::rdf::RDFValue> LightResolver::getProperty(aff4::Lexicon resource) noexcept {
	return AFF4Resource::getProperty(resource);
}

} /* namespace resolver */
} /* namespace aff4 */
