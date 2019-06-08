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

#ifndef TESTS_COMPRESSION_H_
#define TESTS_COMPRESSION_H_

#include <cppunit/extensions/HelperMacros.h>

#include "../aff4config.h"
#include "../src/aff4.h"
#include "../src/codec/CompressionCodec.h"

#include "../src/codec/CompressionCodec.h"
#include "../src/codec/NullCompression.h"
#include "../src/codec/SnappyCompression.h"
#include "../src/codec/DeflateCompression.h"
#include "../src/codec/LZ4Compression.h"
#include "../src/codec/ZlibCompression.h"

#include "zlib.h"
#include "snappy.h"
#include "lz4.h"

#include "TestUtilities.h"

#include <inttypes.h>
#include <string.h>

class compression: public CPPUNIT_NS::TestFixture {

CPPUNIT_TEST_SUITE(compression);

	/*
	 * Generate compression codec from resource type.
	 */
	CPPUNIT_TEST(testSnappy);
	CPPUNIT_TEST(testSnappyResource);
	CPPUNIT_TEST(testLZ4);
	CPPUNIT_TEST(testLZ4Resource);
	CPPUNIT_TEST(testDeflate);
	CPPUNIT_TEST(testDeflateResource);
	CPPUNIT_TEST(testZlib);
	CPPUNIT_TEST(testZlibResource);
	CPPUNIT_TEST(testNull);
	CPPUNIT_TEST(testNullResource);
	CPPUNIT_TEST(testBadValue);
	CPPUNIT_TEST(testBadValueResource);
	CPPUNIT_TEST(testNullValue);
	CPPUNIT_TEST(testNullValueResource);

	/*
	 * Compression tests
	 */
	CPPUNIT_TEST(testNoCompression);
	CPPUNIT_TEST(testSnappyCompression);
	CPPUNIT_TEST(testDeflateCompression);
	CPPUNIT_TEST(testLZ4Compression);
	CPPUNIT_TEST(testZlibCompression);

	CPPUNIT_TEST_SUITE_END()
	;

public:
	compression();
	virtual ~compression();
	void setUp();
	void tearDown();

private:
	/*
	 * Generate compression codec from resource type.
	 */
	void testSnappy();
	void testSnappyResource();
	void testLZ4();
	void testLZ4Resource();
	void testDeflate();
	void testDeflateResource();
	void testZlib();
	void testZlibResource();
	void testNull();
	void testNullResource();
	void testBadValue();
	void testBadValueResource();
	void testNullValue();
	void testNullValueResource();

	/*
	 * Compression tests
	 */
	void testNoCompression();
	void testSnappyCompression();
	void testDeflateCompression();
	void testLZ4Compression();
	void testZlibCompression();

};

#endif /* TESTS_COMPRESSION_H_ */
