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

#ifndef TEST_CONTAINER_H
#define TEST_CONTAINER_H

#include <cppunit/extensions/HelperMacros.h>

#include "../aff4config.h"
#include "../src/aff4.h"
#include "../src/zip/Zip.h"
#include "../src/zip/ZipStream.h"
#include "../src/container/AFF4ZipContainer.h"

#include "TestUtilities.h"

#include <inttypes.h>
#include <string.h>

class container: public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(container);

	CPPUNIT_TEST(testContainerLinear);
	CPPUNIT_TEST(testContainerAllocated);
	CPPUNIT_TEST(testContainerLinearReadError);
	CPPUNIT_TEST(testContainerLinearAllHashes);
	CPPUNIT_TEST(testZipLinear);
	CPPUNIT_TEST(testZipAllocated);
	CPPUNIT_TEST(testZipSegmentRead);

	CPPUNIT_TEST(testContainerDescription);
	CPPUNIT_TEST(testContainerMissingResource);
	CPPUNIT_TEST(testContainerMapContents);
	CPPUNIT_TEST(testContainerImageStreamContents);

	CPPUNIT_TEST(testBlank);
	CPPUNIT_TEST(testBlank5);

	CPPUNIT_TEST_SUITE_END()
	;

public:
	container();
	virtual ~container();
	void setUp();
	void tearDown();

private:
	void testZipLinear();
	void testZipAllocated();
	void testZipSegmentRead();
	void testContainerLinear();
	void testContainerAllocated();
	void testContainerLinearReadError();
	void testContainerLinearAllHashes();

	void testContainerDescription();
	void testContainerMissingResource();
	void testContainerMapContents();
	void testContainerImageStreamContents();

	void testBlank();
	void testBlank5();
};

#endif /* TEST_CONTAINER_H */
