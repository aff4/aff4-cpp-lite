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
 * @file StringUtil.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief Basic String utility functions
 */

#ifndef SRC_UTILS_STRINGUTIL_H_
#define SRC_UTILS_STRINGUTIL_H_

#include "aff4config.h"
#include "aff4.h"

#include <string>

namespace aff4 {

/**
 * @brief General Utility functions.
 */
namespace util {

/**
 * Generate a new random AFF4 ID
 * @return A new random AFF4 ID.
 */
LIBAFF4_API std::string generateID();

/**
 * Does the string has the given ending
 * @param value The filename
 * @param ext the extension.
 * @return TRUE if the string has the given extension.
 */
LIBAFF4_API_LOCAL bool hasSuffix(std::string const &value, std::string const &ext);

/**
 * Does the string has the given starting string
 * @param value The filename
 * @param prefix the prefix.
 * @return TRUE if the string has the given extension.
 */
LIBAFF4_API_LOCAL bool hasPrefix(std::string const &value, std::string const &prefix);

/**
 * Correctly encode the name URL for zip containers.
 *
 * @param url The URL to encode
 * @return The encoded url
 */
LIBAFF4_API_LOCAL std::string nameCodecEncode(std::string url) noexcept;

/**
* Convert UTF-8 to UTF-16 string.
*
* @param str The string to re-encode
* @return The string as UTF-16
*/
LIBAFF4_API_LOCAL std::wstring s2ws(const std::string& str);

/**
* Convert UTF-16 to UTF-8 string.
*
* @param wstr The string to re-encode
* @return The string as UTF-8
*/
LIBAFF4_API_LOCAL std::string ws2s(const std::wstring& wstr);

}/* namespace util */
}/* namespace aff4 */

#endif /* SRC_UTILS_STRINGUTIL_H_ */
