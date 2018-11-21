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

#ifndef TESTS_STREAMS_H_
#define TESTS_STREAMS_H_

#include <cppunit/extensions/HelperMacros.h>

#include "../aff4config.h"
#include "../src/aff4.h"
#include "../src/stream/ImageStreamFactory.h"
#include "../src/container/AFF4ZipContainer.h"

#include "TestUtilities.h"

#include <inttypes.h>
#include <climits>

class streams: public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(streams);
	/*
	 * Symbolic Stream
	 */
	CPPUNIT_TEST(testZeroCreate);
	CPPUNIT_TEST(testZeroRead);
	CPPUNIT_TEST(testCreateSymbolicStream00);
	CPPUNIT_TEST(testReadSymbolicStream00);
	CPPUNIT_TEST(testReadSymbolicStream01);
	CPPUNIT_TEST(testCreateSymbolicViaResource);
	CPPUNIT_TEST(testCreateSymbolicViaResourceAllValues);
	CPPUNIT_TEST(testCreateSymbolicAndReadAllValues);
	CPPUNIT_TEST(testReadSymbolicNullBuffer);

	/*
	 * Unreadable stream
	 */
	CPPUNIT_TEST(testCreateUnreadable);
	CPPUNIT_TEST(testReadUnreadable);
	CPPUNIT_TEST(testReadUnreadable1MB);
	CPPUNIT_TEST(testReadUnreadableOverlap1MB);
	CPPUNIT_TEST(testReadUnreadableOverlap2MB);
	CPPUNIT_TEST(testReadUnreadableOverlap3MB);
	CPPUNIT_TEST(testReadUnreadableNullBuffer);
	CPPUNIT_TEST(testReadUnreadableEmptyBuffer);

	/*
	 * Unknown stream
	 */
	CPPUNIT_TEST(testCreateUnknown);
	CPPUNIT_TEST(testReadUnknown);
	CPPUNIT_TEST(testReadUnknown1MB);
	CPPUNIT_TEST(testReadUnknownOverlap1MB);
	CPPUNIT_TEST(testReadUnknownOverlap2MB);
	CPPUNIT_TEST(testReadUnknownOverlap3MB);
	CPPUNIT_TEST(testReadUnknownNullBuffer);
	CPPUNIT_TEST(testReadUnknownEmptyBuffer);

	/*
	 * aff4:ImageStream
	 */
	CPPUNIT_TEST(testLinearImageStreamContents);
	CPPUNIT_TEST(testAllocatedImageStreamContents);
	CPPUNIT_TEST(testReadErrorImageStreamContents);
	CPPUNIT_TEST(testAllHashsImageStreamContents);

	CPPUNIT_TEST(testMicro7ImageStreamContents);
	CPPUNIT_TEST(testMicro9ImageStreamContents);

	CPPUNIT_TEST_SUITE_END()
	;

public:
	streams();
	virtual ~streams();
	void setUp();
	void tearDown();

private:
	/*
	 * Symbolic Stream
	 */
	void testZeroCreate();
	void testZeroRead();
	void testCreateSymbolicStream00();
	void testReadSymbolicStream00();
	void testReadSymbolicStream01();
	void testCreateSymbolicViaResource();
	void testCreateSymbolicViaResourceAllValues();
	void testCreateSymbolicAndReadAllValues();
	void testReadSymbolicNullBuffer();

	/*
	 * Unreadable stream
	 */
	void testCreateUnreadable();
	void testReadUnreadable();
	void testReadUnreadable1MB();
	void testReadUnreadableOverlap1MB();
	void testReadUnreadableOverlap2MB();
	void testReadUnreadableOverlap3MB();
	void testReadUnreadableNullBuffer();
	void testReadUnreadableEmptyBuffer();

	/*
	 * Unknown stream
	 */
	void testCreateUnknown();
	void testReadUnknown();
	void testReadUnknown1MB();
	void testReadUnknownOverlap1MB();
	void testReadUnknownOverlap2MB();
	void testReadUnknownOverlap3MB();
	void testReadUnknownNullBuffer();
	void testReadUnknownEmptyBuffer();

	/*
	 * aff4:ImageStream
	 */
	void testLinearImageStreamContents();
	void testAllocatedImageStreamContents();
	void testReadErrorImageStreamContents();
	void testAllHashsImageStreamContents();
	void testMicro7ImageStreamContents();
	void testMicro9ImageStreamContents();
};

#endif /* TESTS_STREAMS_H_ */
