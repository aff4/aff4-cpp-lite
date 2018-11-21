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

#if defined _WIN32 && defined _MSC_VER  

 /*
 * MS CPPUNIT
 */

#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "aff4.h"
#include "aff4-c.h"
#include "utils\Cache.h"
#include <functional>

#define CPPUNIT_ASSERT Assert::IsTrue
#define CPPUNIT_ASSERT_EQUAL Assert::AreEqual

namespace AFF4UnitTests
{
	TEST_CLASS(TestVersion)
	{
	public:


#else

 /*
  * CPPUNIT
  */

#include "cacheTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(cacheTest);

cacheTest::cacheTest() {
}

cacheTest::~cacheTest() {
}

void cacheTest::setUp() {
}

void cacheTest::tearDown() {
}

#define TEST_METHOD(x) void cacheTest::x()

#endif

/**
 * Int loader.
 */
class IntLoader {
public:
	IntLoader(uint32_t multipler = 2) :
		multipler(multipler) {
	}

	uint32_t load(uint8_t key) {
		return ((uint32_t)key) * 2 + 1;
	}
protected:
	uint32_t multipler;

};


typedef typename std::pair<std::shared_ptr<uint8_t>, uint32_t> buffer_t;

/**
* buffer loader.
*/
class BufferLoader {
public:

	BufferLoader(uint32_t sz = 32 * 1024) :
		sz(sz) {
	}

	uint32_t sz;

	buffer_t getBuffer(uint64_t key) {
		(void)key;
		sz++;
		std::shared_ptr<uint8_t> buf(new uint8_t[sz], std::default_delete<uint8_t[]>());
		uint32_t size = sz;
		return std::make_pair(buf, size);
	}

};

TEST_METHOD(testIntInt) {

	IntLoader loader(10);
	IntLoader loader2();

	// bind parameters : The method pointer, the instance of the class, placeholder of passed argument (always _1);
	std::function<uint32_t(uint8_t)> loaderFunc = std::bind(&IntLoader::load, &loader, std::placeholders::_1);

	// Create cache with at most 10 elements.
	std::unique_ptr<aff4::util::cache<uint8_t, uint32_t>> c(new aff4::util::cache<uint8_t, uint32_t>(10, loaderFunc));

	CPPUNIT_ASSERT_EQUAL((uint64_t)0, c->size());
	CPPUNIT_ASSERT(!c->exists(1));

	uint32_t value = c->get(1);
	CPPUNIT_ASSERT_EQUAL((uint32_t)3, value);
	CPPUNIT_ASSERT_EQUAL((uint64_t)1, c->size());

	uint64_t sz = c->size();
	for (uint8_t i = 5; i < 25; i += 5) {
		uint32_t expected = loader.load(i);
		value = c->get(i);
		sz++;
		CPPUNIT_ASSERT_EQUAL(expected, value);
		CPPUNIT_ASSERT_EQUAL(sz, c->size());
	}

	CPPUNIT_ASSERT(!c->exists(6));

	/*
	 * Invalidate the cache.
	 */
	c->invalidate();
	CPPUNIT_ASSERT_EQUAL((uint64_t)0, c->size());
	sz = 0;
	for (uint8_t i = 0; i < 10; i++) {
		uint32_t expected = loader.load(i);
		value = c->get(i);
		sz++;
		CPPUNIT_ASSERT_EQUAL(expected, value);
		CPPUNIT_ASSERT_EQUAL(sz, c->size());
	}

	/*
	 * Have a full cache.
	 */
	CPPUNIT_ASSERT_EQUAL((uint64_t)10, c->size());
	sz = 10;
	for (uint8_t i = 20; i < 50; i++) {
		uint32_t expected = loader.load(i);
		value = c->get(i);
		CPPUNIT_ASSERT_EQUAL(expected, value);
		CPPUNIT_ASSERT_EQUAL(sz, c->size());
	}
	// check for evicted entries.
	for (uint8_t i = 0; i < 10; i++) {
		CPPUNIT_ASSERT(!c->exists(i));
	}
	// check for expected entries.
	for (uint8_t i = 40; i < 50; i++) {
		CPPUNIT_ASSERT(c->exists(i));
	}
}


TEST_METHOD(testLongBuffer) {

	uint32_t bufferSize = 128 * 1024;
	BufferLoader loader(bufferSize);

	bufferSize++;
	// bind parameters : The method pointer, the instance of the class, placeholder of passed argument (always _1);
	std::function<buffer_t(uint64_t)> loaderFunc = std::bind(&BufferLoader::getBuffer, &loader, std::placeholders::_1);

	// Create cache with at most 10 elements.
	std::unique_ptr<aff4::util::cache<uint64_t, buffer_t>> c(new aff4::util::cache<uint64_t, buffer_t>(10, loaderFunc));

	CPPUNIT_ASSERT_EQUAL((uint64_t)0, c->size());
	CPPUNIT_ASSERT(!c->exists(1));

	std::pair<std::shared_ptr<uint8_t>, uint32_t> value = c->get(1);
	CPPUNIT_ASSERT_EQUAL((uint32_t)bufferSize++, value.second);
	CPPUNIT_ASSERT_EQUAL((uint64_t)1, c->size());

	uint64_t sz = c->size();
	for (uint8_t i = 5; i < 25; i += 5) {
		value = c->get(i);
		sz++;
		CPPUNIT_ASSERT_EQUAL((uint32_t)bufferSize++, value.second);
		CPPUNIT_ASSERT_EQUAL(sz, c->size());
	}

	CPPUNIT_ASSERT(!c->exists(6));

	/*
	 * Invalidate the cache.
	 */
	c->invalidate();
	CPPUNIT_ASSERT_EQUAL((uint64_t)0, c->size());
	sz = 0;
	for (uint8_t i = 0; i < 10; i++) {
		value = c->get(i);
		sz++;
		CPPUNIT_ASSERT_EQUAL((uint32_t)bufferSize++, value.second);
		CPPUNIT_ASSERT_EQUAL(sz, c->size());
	}

	/*
	 * Have a full cache.
	 */
	CPPUNIT_ASSERT_EQUAL((uint64_t)10, c->size());
	sz = 10;
	for (uint8_t i = 20; i < 50; i++) {
		value = c->get(i);
		CPPUNIT_ASSERT_EQUAL((uint32_t)bufferSize++, value.second);
		// We should never have more than 10 entries.
		CPPUNIT_ASSERT_EQUAL(sz, c->size());
	}
	// check for evicted entries.
	for (uint8_t i = 0; i < 10; i++) {
		CPPUNIT_ASSERT(!c->exists(i));
	}
	// check for expected entries.
	for (uint8_t i = 40; i < 50; i++) {
		CPPUNIT_ASSERT(c->exists(i));
	}
}

#if defined _WIN32 && defined _MSC_VER 

	};

}

#endif
