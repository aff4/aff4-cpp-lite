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
#include "../src/utils/Cache.h"

#include "TestUtilities.h"

#include <inttypes.h>
#include <string.h>
#include <mutex>
#include <memory>

class cacheTest: public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(cacheTest);

	CPPUNIT_TEST(testIntInt);
	CPPUNIT_TEST(testLongBuffer);

	CPPUNIT_TEST_SUITE_END()
	;

public:
	cacheTest();
	virtual ~cacheTest();
	void setUp();
	void tearDown();

private:
	void testIntInt();
	void testLongBuffer();

};

#endif /* TEST_CONTAINER_H */
