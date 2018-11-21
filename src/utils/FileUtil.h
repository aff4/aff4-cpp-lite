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

/**
 * @file FileUtil.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief Basic File utility functions
 */

#ifndef SRC_UTILS_FILEUTIL_H_
#define SRC_UTILS_FILEUTIL_H_

#include "aff4config.h"
#include "aff4.h"

#include "StringUtil.h"
#include <sys/stat.h>

#ifndef _WIN32
#include <unistd.h>
#include <dirent.h>
#endif

#include <sys/types.h>
#include <string>

namespace aff4 {

/**
 * @brief General Utility functions.
 */
namespace util {

/**
 * Does the file entity exist. (This does NOT check the type of entity)
 * @param name The filename to check
 * @return TRUE if the file entity exists.
 */
LIBAFF4_API_LOCAL inline bool fileExists(const std::string& name) {
#ifndef _WIN32
	/*
	* POSIX based systems.
	*/
	struct stat buffer;
	return (::stat(name.c_str(), &buffer) == 0);
#else 
	/*
	* Windows based systems
	*/
	std::wstring filename = aff4::util::s2ws(name);
	DWORD dwAttrib = GetFileAttributes(filename.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES);
#endif
}

/**
 * Does the file entity exist, and is a regular file.
 * @param name The filename to check
 * @return TRUE if the file entity exists and is a regular file.
 */
LIBAFF4_API_LOCAL inline bool isFile(const std::string& name) {
#ifndef _WIN32
	/*
	* POSIX based systems.
	*/
	struct stat buffer;
	if (::stat(name.c_str(), &buffer) == 0) {
		return S_ISREG(buffer.st_mode);
	}
	return false;
#else 
	/*
	* Windows based systems
	*/
	std::wstring filename = aff4::util::s2ws(name);
	DWORD dwAttrib = GetFileAttributes(filename.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

#endif
}

/**
 * Does the file entity exist and is a directory.
 * @param name The filename to check
 * @return TRUE if the file entity exists and is a directory.
 */
LIBAFF4_API_LOCAL inline bool isDirectory(const std::string& name) {
#ifndef _WIN32
	/*
	* POSIX based systems.
	*/
	struct stat buffer;
	if (::stat(name.c_str(), &buffer) == 0) {
		return S_ISDIR(buffer.st_mode);
	}

#else 
	/*
	* Windows based systems
	*/
	std::wstring filename = aff4::util::s2ws(name);
	DWORD dwAttrib = GetFileAttributes(filename.c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));

#endif
	return false;
}

}/* namespace util */
}/* namespace aff4 */

#endif /* SRC_UTILS_FILEUTIL_H_ */
