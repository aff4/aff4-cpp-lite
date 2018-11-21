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

#ifndef TESTS_IMAGE_H_
#define TESTS_IMAGE_H_

#include <cppunit/extensions/HelperMacros.h>

#include "../aff4config.h"
#include "../src/aff4.h"
#include "../src/aff4-c.h"
#include "../src/container/AFF4ZipContainer.h"

#include "TestUtilities.h"

#include <inttypes.h>
#include <climits>

class image: public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(image);

	// Metadata
	CPPUNIT_TEST(testContainerLinear);
	CPPUNIT_TEST(testContainerAllocated);
	CPPUNIT_TEST(testContainerLinearReadError);
	CPPUNIT_TEST(testContainerLinearAllHashes);

	// Basic Images
	CPPUNIT_TEST(testLinearImageStreamContents);
	CPPUNIT_TEST(testAllocatedImageStreamContents);
	CPPUNIT_TEST(testReadErrorImageStreamContents);
	CPPUNIT_TEST(testAllHashsImageStreamContents);
	CPPUNIT_TEST(testContainerAllocatedUnknown);

	// Physical Memory Images.
	CPPUNIT_TEST(testContainer7);
	CPPUNIT_TEST(testContainer9);

	// Stiped Images.
	CPPUNIT_TEST(testContainerLinear_Part1);
	CPPUNIT_TEST(testContainerLinear_Part2);

	// C API
	CPPUNIT_TEST(testCAPI_Linear);
	CPPUNIT_TEST(testCAPI_Allocated);
	CPPUNIT_TEST(testCAPI_Striped);
	CPPUNIT_TEST(testCAPI_PMem);

	// Unicode conversion
	// CPPUNIT_TEST(testUnicodeFilename);
	// CPPUNIT_TEST(testUnicodeFilename2);

	CPPUNIT_TEST_SUITE_END()
	;

public:
	image();
	virtual ~image();
	void setUp();
	void tearDown();

private:
	/*
	 * Metadata tests
	 */
	void testContainerLinear();
	void testContainerAllocated();
	void testContainerLinearReadError();
	void testContainerLinearAllHashes();

	/*
	 * aff4:Stream tests
	 */
	void testLinearImageStreamContents();
	void testAllocatedImageStreamContents();
	void testReadErrorImageStreamContents();
	void testAllHashsImageStreamContents();
	void testContainerAllocatedUnknown();

	/*
	 * Physical Memory images.
	 */
	void testContainer7();
	void testContainer9();

	/*
	 * Striped images.
	 */
	void testContainerLinear_Part1();
	void testContainerLinear_Part2();

	/*
	 * C API test
	 */
	void testCAPI_Linear();
	void testCAPI_Allocated();
	void testCAPI_Striped();
	void testCAPI_PMem();

	/*
	 * Unicode conversions.
	 */
	void testUnicodeFilename();
	void testUnicodeFilename2();

};

#endif /* TESTS_IMAGE_H_ */
