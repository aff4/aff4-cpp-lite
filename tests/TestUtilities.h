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

#ifndef TESTS_TESTUTILITIES_H_
#define TESTS_TESTUTILITIES_H_

#ifndef _WIN32
#include "../src/aff4.h"
#include "../src/aff4-c.h"
#include <cppunit/extensions/HelperMacros.h>
#else 
#include "CppUnitTest.h"
#include "aff4.h"
#include "aff4-c.h"
#endif

#include <algorithm>
#include <inttypes.h>
#include <openssl/sha.h>

namespace aff4 {
namespace test {

/**
 * Print all resource properties to console.
 * @param image The resource to dump to console.
 */
void printResourceProperties(std::shared_ptr<aff4::IAFF4Resource> image);
/**
 * Test that the given resource has the given property
 * @param image The resource
 * @param property The property
 * @param expected The expected value.
 */
void testProperty(std::shared_ptr<aff4::IAFF4Resource> image, aff4::Lexicon property, std::string expected);
/**
 * Test that the given resource has the given property
 * @param image The resource
 * @param property The property
 * @param expected The expected value.
 */
void testProperty(std::shared_ptr<aff4::IAFF4Resource> image, aff4::Lexicon property, aff4::Lexicon expected);
/**
 * Test that the given resource has the given property
 * @param image The resource
 * @param property The property
 * @param expected The expected value.
 */
void testProperty(std::shared_ptr<aff4::IAFF4Resource> image, aff4::Lexicon property, int32_t expected);
/**
 * Test that the given resource has the given property
 * @param image The resource
 * @param property The property
 * @param expected The expected value.
 */
void testProperty(std::shared_ptr<aff4::IAFF4Resource> image, aff4::Lexicon property, int64_t expected);
/**
 * Test that the given resource has the given property stored as a resource.
 * @param image The resource
 * @param property The property
 * @param expected The expected value.
 */
void testPropertyResource(std::shared_ptr<aff4::IAFF4Resource> image, aff4::Lexicon property, std::string expected);

/**
 * Calculate the SHA1 sum of the stream
 * @param stream The stream to construct the digest.
 * @param readSize The size of the read to use when reading from the stream. (default = 128K)
 * @return The digest.
 */
std::string sha1sum(std::shared_ptr<IAFF4Stream> stream, uint64_t readSize = 128 * 1024);
/**
 * Calculate the SHA1 sum of the stream via C API.
 * @param handle The stream to construct the digest.
 * @param toRead The size of the stream.
 * @param readSize The size of the read to use when reading from the stream. (default = 128K)
 * @return The digest.
 */
std::string sha1sum(int handle, uint64_t toRead, uint64_t readSize = 128 * 1024);

} /* namespace test */
} /* namespace aff4 */

#endif /* TESTS_TESTUTILITIES_H_ */
