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

#ifndef TESTS_RESOLVER_H_
#define TESTS_RESOLVER_H_

#include <cppunit/extensions/HelperMacros.h>

#include "../aff4config.h"
#include "../src/aff4.h"
#include "../src/utils/StringUtil.h"

#include "TestUtilities.h"

#include <inttypes.h>
#include <string.h>

class resolver: public CPPUNIT_NS::TestFixture {
CPPUNIT_TEST_SUITE(resolver);

	CPPUNIT_TEST(generateIDs);
	CPPUNIT_TEST(testBaseFolder);
	CPPUNIT_TEST(testBaseFolderFile);
	CPPUNIT_TEST(testStripedFolder);
	CPPUNIT_TEST(testStripedFolderFile);
	CPPUNIT_TEST(testEmptyFolder);
	CPPUNIT_TEST(testNotExistFolder);

	CPPUNIT_TEST_SUITE_END()
	;

public:
	resolver();
	virtual ~resolver();
	void setUp();
	void tearDown();

private:
	void generateIDs();
	void testBaseFolder();
	void testBaseFolderFile();
	void testStripedFolder();
	void testStripedFolderFile();
	void testEmptyFolder();
	void testNotExistFolder();
};

#endif /* TESTS_RESOLVER_H_ */
