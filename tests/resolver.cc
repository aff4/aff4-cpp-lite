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
#include "utils\StringUtil.h"

#define CPPUNIT_ASSERT Assert::IsTrue
#define CPPUNIT_ASSERT_EQUAL Assert::AreEqual

#define UNITTEST_BASE_PATH "..\\..\\..\\"

namespace AFF4UnitTests
{
	TEST_CLASS(TestResolver)
	{
	public:


#else

 /*
 * CPPUNIT
 */

#include "resolver.h"

CPPUNIT_TEST_SUITE_REGISTRATION(resolver);

resolver::resolver() {
}

resolver::~resolver() {
}

void resolver::setUp() {
}

void resolver::tearDown() {
}

#define TEST_METHOD(x) void resolver::x()
#define UNITTEST_BASE_PATH

#endif

#ifdef _WIN32
#define UNITTEST_PATH "\\"
#else
#define UNITTEST_PATH "/"
#endif

TEST_METHOD(generateIDs) {
	std::string last;
	for(int i = 0; i < 16; i++){
		std::string resource = aff4::util::generateID();
		printf("%d : %s\n", i, resource.c_str());
		if(!last.empty()){
			CPPUNIT_ASSERT(!(last.compare(resource) == 0));
		}
		last = resource;
	}
	CPPUNIT_ASSERT(true);
}

TEST_METHOD(testBaseFolder) {
	std::string path(UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources");

	std::unique_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(path));
	CPPUNIT_ASSERT(resolver.get() != nullptr);
	CPPUNIT_ASSERT(resolver->hasResource("aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044"));
	CPPUNIT_ASSERT(resolver->hasResource("aff4://ce24a0d0-a540-442a-939e-938b848add9a"));
	CPPUNIT_ASSERT(resolver->hasResource("aff4://686e3512-b568-48fd-ac7b-73764b98a9aa"));
	CPPUNIT_ASSERT(resolver->hasResource("aff4://7a86cb01-217c-4852-b8e0-c94be1ca5ac5"));
	CPPUNIT_ASSERT(!resolver->hasResource("aff4://685e15cc-d0fb-4dbc-ba47-48117fc77045"));
}

TEST_METHOD(testBaseFolderFile) {
	std::string path(UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Base-Linear.aff4");

	std::unique_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(path));
	CPPUNIT_ASSERT(resolver.get() != nullptr);
	CPPUNIT_ASSERT(resolver->hasResource("aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044"));
	CPPUNIT_ASSERT(resolver->hasResource("aff4://ce24a0d0-a540-442a-939e-938b848add9a"));
	CPPUNIT_ASSERT(resolver->hasResource("aff4://686e3512-b568-48fd-ac7b-73764b98a9aa"));
	CPPUNIT_ASSERT(resolver->hasResource("aff4://7a86cb01-217c-4852-b8e0-c94be1ca5ac5"));
	CPPUNIT_ASSERT(!resolver->hasResource("aff4://685e15cc-d0fb-4dbc-ba47-48117fc77045"));
}

TEST_METHOD(testStripedFolder) {
	std::string path(UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Striped");

	std::unique_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(path));
	CPPUNIT_ASSERT(resolver.get() != nullptr);
	CPPUNIT_ASSERT(!resolver->hasResource("aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044"));
	CPPUNIT_ASSERT(resolver->hasResource("aff4://7cbb47d0-b04c-42bc-8c04-87b7782739ad"));
	CPPUNIT_ASSERT(resolver->hasResource("aff4://51725cd9-3769-4be7-a8ab-94e3ea62bf9a"));
}

#ifdef _WIN32
#define UNITTEST_PATH "\\"
#else
#define UNITTEST_PATH "/"
#endif

TEST_METHOD(testStripedFolderFile) {
	std::string path(UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Striped" UNITTEST_PATH "Base-Linear_1.aff4");

	std::unique_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(path));
	CPPUNIT_ASSERT(resolver.get() != nullptr);
	CPPUNIT_ASSERT(!resolver->hasResource("aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044"));
	CPPUNIT_ASSERT(resolver->hasResource("aff4://7cbb47d0-b04c-42bc-8c04-87b7782739ad"));
	CPPUNIT_ASSERT(resolver->hasResource("aff4://51725cd9-3769-4be7-a8ab-94e3ea62bf9a"));
}

TEST_METHOD(testEmptyFolder) {
	std::string path(UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Empty");

	std::unique_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(path));
	CPPUNIT_ASSERT(resolver.get() != nullptr);
	CPPUNIT_ASSERT(!resolver->hasResource("aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044"));
	CPPUNIT_ASSERT(!resolver->hasResource("aff4://7cbb47d0-b04c-42bc-8c04-87b7782739ad"));
	CPPUNIT_ASSERT(!resolver->hasResource("aff4://51725cd9-3769-4be7-a8ab-94e3ea62bf9a"));
}

TEST_METHOD(testNotExistFolder) {
	std::string path(UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "NULL");

	std::unique_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(path));
	CPPUNIT_ASSERT(resolver == nullptr);
}

#if defined _WIN32 && defined _MSC_VER 

	};

}

#endif
