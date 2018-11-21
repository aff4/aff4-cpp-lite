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

#pragma once

#include "stdafx.h"
#include "xwaysPlugin.h"

#include <locale>
#include <codecvt>

#ifndef CRLF
#define CRLF "\r\n"
#endif

/**
 * Convert UTF-8 to UTF-16 String
 */
std::wstring s2ws(const std::string& str);

/**
* Convert UTF-16 to UTF-8 String
*/
std::string ws2s(const std::wstring& wstr);

/**
* Copy contents of UTF-16 string to newly created buffer and return pointer to buffer. (String is NULL terminated).
*/
LPWSTR createText(const std::wstring& value);

/**
* Get string metadata for image. (new line separator for fields).
*/
LPWSTR getMetaData(std::shared_ptr<aff4::IAFF4Image> image);

/**
* Is the given range in the stream sparse. 
* 
* Sparse is defined as aff4:Zero, aff4:Unknown, aff4:Unreadable.
*/
bool isSparse(std::shared_ptr<aff4::IAFF4Stream> stream, INT64 offset, INT64 size);
