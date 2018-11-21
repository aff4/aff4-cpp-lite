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

#ifndef EXAMPLES_UTILS_H_
#define EXAMPLES_UTILS_H_

#include "aff4.h"

#ifdef _WIN32
#include "stdafx.h"
#endif

#include <vector>
#include <iostream>
#include <openssl/sha.h>

/**
 * Does the string has the given starting string
 * @param value The filename
 * @param prefix the prefix.
 * @return TRUE if the string has the given extension.
 */
bool hasPrefix(std::string const &value, std::string const &prefix);

/**
 * Print all values.
 * @param values The values to print.
 */
void printElements(const std::vector<aff4::rdf::RDFValue>& values);

/**
 * Print all elements of this resource
 * @param resource The resource
 * @param spacer The spacer for displaying values...
 */
void printProperties(std::shared_ptr<aff4::IAFF4Resource> resource, const std::string& spacer);

/**
 * Get the sha1sum of the stream
 * @param stream The stream to read.
 * @param readSize The default read size
 * @return The sha1sum value.
 */
std::string sha1sum(std::shared_ptr<aff4::IAFF4Stream> stream, uint64_t readSize);

#endif /* EXAMPLES_UTILS_H_ */
