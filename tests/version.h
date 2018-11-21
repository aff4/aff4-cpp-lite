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

#ifndef TEST_VERSION_H
#define TEST_VERSION_H

#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#ifndef _WIN32
#include "../src/aff4.h"
#include "../src/aff4-c.h"
#else 
#include "aff4.h"
#include "aff4-c.h"
#endif

class version : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(version);

    CPPUNIT_TEST(testVersionString);
    CPPUNIT_TEST(testCVersionString);

    CPPUNIT_TEST_SUITE_END();

public:
    version();
    virtual ~version();
    void setUp();
    void tearDown();

private:
    void testVersionString();
    void testCVersionString();
};

#endif /* TEST_VERSION_H */
