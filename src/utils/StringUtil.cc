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

#include "StringUtil.h"
#include <random>
#include <ctime>
#include <cstdio>
#include <locale>
#include <codecvt>

namespace aff4 {
namespace util {

std::wstring s2ws(const std::string& str) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}

bool hasSuffix(std::string const &value, std::string const &ext) {
	// Ensure the filename is longer than the ext.
	if (value.length() >= ext.length()) {
		return (!value.compare(value.length() - ext.length(), ext.length(), ext));
	} else {
		return false;
	}
}

bool hasPrefix(std::string const &value, std::string const &prefix) {
	if (value.length() >= prefix.length()) {
		return (!value.compare(0, prefix.length(), prefix));
	} else {
		return false;
	}
}

std::string nameCodecEncode(std::string url) noexcept {
	if (hasPrefix(url, "aff4://")) {
		url = "aff4%3A%2F%2F" + url.substr(7);
	}
	return url;
}

std::string generateID() {
	/*
	 * Create: "aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044"
	 *         "aff4://25fff12c-fa32-4e70-9fa9-07ca8e6c1b6f"
	 * Note: The following is NASTY, and should be using a standard library.
	 */
#if defined _WIN32 && defined _MSC_VER  
	 /*
	 * use std::random_device for production
	 * Note: early versions of valgrind will die when using std::random_device.
	 */
	std::random_device rd;
	std::mt19937 mt(rd());
#else 
	/*
	 * Use current time for testing when DEBUG is true.
	 */
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	auto duration = now.time_since_epoch();
	std::mt19937 mt(duration.count());

#endif
	std::uniform_int_distribution<int> dist(0, 255);
	// generate our values.
	int values[16];
	for (int i = 0; i < 16; i++) {
		values[i] = dist(mt);
	}
	// generate the string
	char res[44];
	std::snprintf(res, 44, AFF4_URN_PREFIX "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x", //
			values[0], values[1], values[2], values[3], values[4], values[5], values[6], values[7], //
			values[8], values[9], values[10], values[11], values[12], values[13], values[14], values[15]);
	std::string resource(res, 43);
	return resource;
}

}/* namespace util */
}/* namespace aff4 */

