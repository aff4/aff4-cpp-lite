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
#include "TestUtilities.h"
#include "stream\ImageStreamFactory.h"
#include "container\AFF4ZipContainer.h"

#define CPPUNIT_ASSERT Assert::IsTrue
#define CPPUNIT_ASSERT_EQUAL Assert::AreEqual

#define UNITTEST_BASE_PATH "..\\..\\..\\"

namespace AFF4UnitTests
{
	TEST_CLASS(TestStreams)
	{
	public:


#else

 /*
 * CPPUNIT
 */

#include "streams.h"

CPPUNIT_TEST_SUITE_REGISTRATION(streams);

streams::streams() {
}

streams::~streams() {
}

void streams::setUp() {
}

void streams::tearDown() {
}

#define TEST_METHOD(x) void streams::x()
#define UNITTEST_BASE_PATH

#endif

void testStreamContents(std::shared_ptr<aff4::IAFF4Stream> stream, std::string expectedSHA1,
	uint64_t readSize) {
	CPPUNIT_ASSERT(stream != nullptr);
	std::string sha1 = aff4::test::sha1sum(stream, readSize);
	printf("%s: %s = %s\n", stream->getResourceID().c_str(), expectedSHA1.c_str(), sha1.c_str());
	CPPUNIT_ASSERT_EQUAL(expectedSHA1, sha1);
}

/*
 * Symbolic Stream
 */
TEST_METHOD(testZeroCreate) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createZeroStream();
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_ZERO),
			stream->getResourceID());

	aff4::test::testProperty(stream, aff4::Lexicon::AFF4_TYPE, aff4::Lexicon::AFF4_IMAGESTREAM_TYPE);
	aff4::test::testProperty(stream, aff4::Lexicon::AFF4_SIZE, (int64_t) LLONG_MAX);

	CPPUNIT_ASSERT_EQUAL((uint64_t)ULLONG_MAX, (uint64_t )stream->size());
	stream->close();
}

TEST_METHOD(testZeroRead) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createZeroStream();
	CPPUNIT_ASSERT(stream != nullptr);

	char buffer[16];
	int64_t res = stream->read(&buffer, 16, 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)16, (uint64_t)res);

	// confirm the contents is as expected.
	for (int i = 0; i < 16; i++) {
		CPPUNIT_ASSERT_EQUAL((char )0, buffer[i]);
	}
}

TEST_METHOD(testCreateSymbolicStream00) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createSymbolicStream(0);
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string(AFF4_BASE_URI "SymbolicStream00"), stream->getResourceID());

	aff4::test::testProperty(stream, aff4::Lexicon::AFF4_TYPE, aff4::Lexicon::AFF4_IMAGESTREAM_TYPE);
	aff4::test::testProperty(stream, aff4::Lexicon::AFF4_SIZE, (int64_t) LLONG_MAX);

	CPPUNIT_ASSERT_EQUAL((uint64_t)ULLONG_MAX, (uint64_t )stream->size());
	stream->close();
}

TEST_METHOD(testReadSymbolicStream00) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createSymbolicStream(0);
	CPPUNIT_ASSERT(stream != nullptr);

	char buffer[16];
	int64_t res = stream->read(&buffer, 16, 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)16, (uint64_t)res);

	// confirm the contents is as expected.
	for (int i = 0; i < 16; i++) {
		CPPUNIT_ASSERT_EQUAL((char )0, buffer[i]);
	}
}

TEST_METHOD(testReadSymbolicStream01) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createSymbolicStream(1);
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string(AFF4_BASE_URI "SymbolicStream01"), stream->getResourceID());
	char buffer[16];
	int64_t res = stream->read(&buffer, 16, 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)16, (uint64_t)res);

	// confirm the contents is as expected.
	for (int i = 0; i < 16; i++) {
		CPPUNIT_ASSERT_EQUAL((char )1, buffer[i]);
	}
}

TEST_METHOD(testCreateSymbolicViaResource) {
	std::string resource(AFF4_BASE_URI "SymbolicStream00");
	std::shared_ptr<aff4::stream::SymbolicImageStream> stream = std::make_shared<aff4::stream::SymbolicImageStream>(
			resource);
	CPPUNIT_ASSERT_EQUAL(std::string(AFF4_BASE_URI "SymbolicStream00"), stream->getResourceID());

	CPPUNIT_ASSERT_EQUAL((uint64_t)ULLONG_MAX, (uint64_t )stream->size());
	CPPUNIT_ASSERT_EQUAL((uint8_t )0, stream->getSymbol());
	stream->close();
}

TEST_METHOD(testCreateSymbolicViaResourceAllValues) {
	for (int symbol = 0; symbol < 256; symbol++) {

		std::string resource = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_SYMBOLIC_PREFIX);
		char buf[3];
		std::snprintf(buf, 3, "%02X", symbol);
		resource += std::string(buf, 2);

		std::shared_ptr<aff4::stream::SymbolicImageStream> stream = std::make_shared<aff4::stream::SymbolicImageStream>(
				resource);
		CPPUNIT_ASSERT_EQUAL(resource, stream->getResourceID());

		CPPUNIT_ASSERT_EQUAL((uint64_t)ULLONG_MAX, (uint64_t )stream->size());
		CPPUNIT_ASSERT_EQUAL(symbol, (int )stream->getSymbol());
		stream->close();
	}
}

TEST_METHOD(testCreateSymbolicAndReadAllValues) {
	char buffer[16];
	for (int symbol = 0; symbol < 256; symbol++) {

		std::string resource = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_SYMBOLIC_PREFIX);
		char buf[3];
		std::snprintf(buf, 3, "%02X", symbol);
		resource += std::string(buf, 2);

		std::shared_ptr<aff4::stream::SymbolicImageStream> stream = std::make_shared<aff4::stream::SymbolicImageStream>(
				resource);
		CPPUNIT_ASSERT_EQUAL(resource, stream->getResourceID());

		CPPUNIT_ASSERT_EQUAL((uint64_t)ULLONG_MAX, (uint64_t )stream->size());
		CPPUNIT_ASSERT_EQUAL(symbol, (int )stream->getSymbol());

		uint64_t res = stream->read(&buffer, 16, 0);
		CPPUNIT_ASSERT_EQUAL(res, (uint64_t )16);

		// confirm the contents is as expected.
		for (int i = 0; i < 16; i++) {
			CPPUNIT_ASSERT_EQUAL((char )symbol, buffer[i]);
		}

		stream->close();
	}
}

TEST_METHOD(testReadSymbolicNullBuffer) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createSymbolicStream(0);
	CPPUNIT_ASSERT(stream != nullptr);
	int64_t res = stream->read(nullptr, 16, 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)0, (uint64_t)res);
	stream->close();
}

/*
 * Unreadable stream
 */
TEST_METHOD(testCreateUnreadable) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnreadableStream();
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_UNREADABLE),
			stream->getResourceID());

	aff4::test::testProperty(stream, aff4::Lexicon::AFF4_TYPE, aff4::Lexicon::AFF4_IMAGESTREAM_TYPE);
	aff4::test::testProperty(stream, aff4::Lexicon::AFF4_SIZE, (int64_t) LLONG_MAX);

	CPPUNIT_ASSERT_EQUAL((uint64_t)ULLONG_MAX, (uint64_t )stream->size());
	stream->close();
}

TEST_METHOD(testReadUnreadable) {
	std::string expected("UNREADABLEDATAUN");
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnreadableStream();
	CPPUNIT_ASSERT(stream != nullptr);
	char buffer[16];
	int64_t res = stream->read(&buffer, 16, 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)16, (uint64_t)res);
	std::string result(buffer, 16);
	CPPUNIT_ASSERT_EQUAL(expected, result);
	stream->close();
}

TEST_METHOD(testReadUnreadable1MB) {
	char expected[] = { 'U', 'N', 'R', 'E', 'A', 'D', 'A', 'B', 'L', 'E', 'D', 'A', 'T', 'A' };
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnreadableStream();
	CPPUNIT_ASSERT(stream != nullptr);
	std::unique_ptr<char[]>buffer(new char[1024 * 1024]);
	int64_t res = stream->read(buffer.get(), 1024 * 1024, 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)res, (uint64_t )1024 * 1024);
	for (int i = 0; i < 1024 * 1024; i++) {
		CPPUNIT_ASSERT_EQUAL(expected[i % 14], buffer[i]);
	}
	stream->close();
}

TEST_METHOD(testReadUnreadableOverlap1MB) {
	std::string expected("REUNREADABLEDATA");
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnreadableStream();
	CPPUNIT_ASSERT(stream != nullptr);
	char buffer[16];
	uint64_t res = stream->read(&buffer, 16, 1024 * 1024 - 2);
	CPPUNIT_ASSERT_EQUAL((uint64_t)16, (uint64_t)res);
	std::string result(buffer, 16);
	CPPUNIT_ASSERT_EQUAL(expected, result);
	stream->close();
}

TEST_METHOD(testReadUnreadableOverlap2MB) {
	std::string expected("REUNREADABLEDATA");
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnreadableStream();
	CPPUNIT_ASSERT(stream != nullptr);
	char buffer[16];
	int64_t res = stream->read(&buffer, 16, 2 * 1024 * 1024 - 2);
	CPPUNIT_ASSERT_EQUAL((uint64_t)16, (uint64_t)res);
	std::string result(buffer, 16);
	CPPUNIT_ASSERT_EQUAL(expected, result);
	stream->close();
}

TEST_METHOD(testReadUnreadableOverlap3MB) {
	std::string expected("REUNREADABLEDATA");
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnreadableStream();
	CPPUNIT_ASSERT(stream != nullptr);
	char buffer[16];
	int64_t res = stream->read(&buffer, 16, 3 * 1024 * 1024 - 2);
	CPPUNIT_ASSERT_EQUAL((uint64_t)16, (uint64_t)res);
	std::string result(buffer, 16);
	CPPUNIT_ASSERT_EQUAL(expected, result);
	stream->close();
}

TEST_METHOD(testReadUnreadableNullBuffer) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnreadableStream();
	CPPUNIT_ASSERT(stream != nullptr);
	int64_t res = stream->read(nullptr, 16, 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)0, (uint64_t)res);
	stream->close();
}

TEST_METHOD(testReadUnreadableEmptyBuffer) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnreadableStream();
	CPPUNIT_ASSERT(stream != nullptr);
	char buffer[16];
	int64_t res = stream->read(&buffer, 0, 1);
	CPPUNIT_ASSERT_EQUAL((uint64_t)0, (uint64_t)res);
	stream->close();
}

/*
 * Unknown stream
 */
TEST_METHOD(testCreateUnknown) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnknownStream();
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_UNKNOWN),
			stream->getResourceID());

	aff4::test::testProperty(stream, aff4::Lexicon::AFF4_TYPE, aff4::Lexicon::AFF4_IMAGESTREAM_TYPE);
	aff4::test::testProperty(stream, aff4::Lexicon::AFF4_SIZE, (int64_t) LLONG_MAX);

	CPPUNIT_ASSERT_EQUAL((uint64_t)ULLONG_MAX, (uint64_t )stream->size());
	stream->close();
}

TEST_METHOD(testReadUnknown) {
	std::string expected("UNKNOWNUNKNOWNUN");
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnknownStream();
	CPPUNIT_ASSERT(stream != nullptr);
	char buffer[16];
	int64_t res = stream->read(&buffer, 16, 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)16, (uint64_t)res);
	std::string result(buffer, 16);
	CPPUNIT_ASSERT_EQUAL(expected, result);
	stream->close();
}

TEST_METHOD(testReadUnknown1MB) {
	char expected[] = { 'U', 'N', 'K', 'N', 'O', 'W', 'N', 'U', 'N', 'K', 'N', 'O', 'W', 'N', 'U', 'N' };
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnknownStream();
	CPPUNIT_ASSERT(stream != nullptr);
	std::unique_ptr<char[]>buffer(new char[1024 * 1024]);
	int64_t res = stream->read(buffer.get(), 1024 * 1024, 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)res, (uint64_t )1024 * 1024);
	for (int i = 0; i < 1024 * 1024; i++) {
		CPPUNIT_ASSERT_EQUAL(expected[i % 14], buffer[i]);
	}
	stream->close();
}
TEST_METHOD(testReadUnknownOverlap1MB) {
	std::string expected("KNUNKNOWNUNKNOWN");
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnknownStream();
	CPPUNIT_ASSERT(stream != nullptr);
	char buffer[16];
	int64_t res = stream->read(&buffer, 16, 1024 * 1024 - 2);
	CPPUNIT_ASSERT_EQUAL((uint64_t)16, (uint64_t)res);
	std::string result(buffer, 16);
	CPPUNIT_ASSERT_EQUAL(expected, result);
	stream->close();
}

TEST_METHOD(testReadUnknownOverlap2MB) {
	std::string expected("KNUNKNOWNUNKNOWN");
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnknownStream();
	CPPUNIT_ASSERT(stream != nullptr);
	char buffer[16];
	int64_t res = stream->read(&buffer, 16, 2 * 1024 * 1024 - 2);
	CPPUNIT_ASSERT_EQUAL((uint64_t)16, (uint64_t)res);
	std::string result(buffer, 16);
	CPPUNIT_ASSERT_EQUAL(expected, result);
	stream->close();
}

TEST_METHOD(testReadUnknownOverlap3MB) {
	std::string expected("KNUNKNOWNUNKNOWN");
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnknownStream();
	CPPUNIT_ASSERT(stream != nullptr);
	char buffer[16];
	int64_t res = stream->read(&buffer, 16, 3 * 1024 * 1024 - 2);
	CPPUNIT_ASSERT_EQUAL((uint64_t)16, (uint64_t)res);
	std::string result(buffer, 16);
	CPPUNIT_ASSERT_EQUAL(expected, result);
	stream->close();
}

TEST_METHOD(testReadUnknownNullBuffer) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnknownStream();
	CPPUNIT_ASSERT(stream != nullptr);
	int64_t res = stream->read(nullptr, 16, 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)0, (uint64_t)res);
	stream->close();
}

TEST_METHOD(testReadUnknownEmptyBuffer) {
	std::shared_ptr<aff4::IAFF4Stream> stream = aff4::stream::createUnknownStream();
	CPPUNIT_ASSERT(stream != nullptr);
	char buffer[16];
	int64_t res = stream->read(&buffer, 0, 1);
	CPPUNIT_ASSERT_EQUAL((uint64_t)0, (uint64_t)res);
	stream->close();
}

/*
 * aff4:ImageStream
 */

const std::string file_1 = UNITTEST_BASE_PATH "tests/resources/Base-Linear.aff4";
const std::string container_1 = "aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044";
const std::string stream_1 = "aff4://c215ba20-5648-4209-a793-1f918c723610";
const std::string streamSHA1_1 = "fbac22cca549310bc5df03b7560afcf490995fbb";

const std::string file_2 = UNITTEST_BASE_PATH "tests/resources/Base-Allocated.aff4";
const std::string container_2 = "aff4://ce24a0d0-a540-442a-939e-938b848add9a";
const std::string stream_2 = "aff4://fce3df71-dce8-4a17-af67-36bed58f25c9";
const std::string streamSHA1_2 = "eb6aa5ba18ec68e94ddc9e7a06871127dcafdaa6";

const std::string file_3 = UNITTEST_BASE_PATH "tests/resources/Base-Linear-ReadError.aff4";
const std::string container_3 = "aff4://686e3512-b568-48fd-ac7b-73764b98a9aa";
const std::string stream_3 = "aff4://4b4396f1-0b68-4be0-af0f-5bf4667fe27b";
const std::string streamSHA1_3 = "ac152b5b8598aef49f21e6e23e8fdc40d2946a6e";

const std::string file_4 = UNITTEST_BASE_PATH "tests/resources/Base-Linear-AllHashes.aff4";
const std::string container_4 = "aff4://7a86cb01-217c-4852-b8e0-c94be1ca5ac5";
const std::string stream_4 = "aff4://e53a108a-bb2e-41f4-ab2e-28fe4ef578c1";
const std::string streamSHA1_4 = "fbac22cca549310bc5df03b7560afcf490995fbb";

const std::string file_5 = UNITTEST_BASE_PATH "tests/resources/Micro7.001.aff4";
const std::string container_5 = "aff4://2ec3bb84-c9fb-4c4e-beba-add19fe60b99";
const std::string stream_5 = "aff4://18bd8b0d-e804-46d1-8977-92ee980f64d9";
const std::string streamSHA1_5 = "a1bc4e8714a83e2e120bab4f3db54ec723e71847";

const std::string file_6 = UNITTEST_BASE_PATH "tests/resources/Micro9.001.aff4";
const std::string container_6 = "aff4://80b64e67-39f1-4564-948a-911f06f6343f";
const std::string stream_6 = "aff4://b399fa7a-0701-4e1f-8233-f134179f1d21";
const std::string streamSHA1_6 = "66873299ad383bc73b24144ef5bd409e618dd25e";

/*
 * Default read sizes for reading the image stream.
 */
const std::vector<uint64_t> readSizes = { //
		512, //
		571, //
		1024, //
		2048, //
		4096, //
		AFF4_DEFAULT_CHUNK_SIZE - 1, //
		AFF4_DEFAULT_CHUNK_SIZE, //
		AFF4_DEFAULT_CHUNK_SIZE + 1, //
		512 * 1024 - 1, //
		512 * 1024, //
		512 * 1024 + 1, //
		1024 * 1024, //
		1024 * 1024 + 1, //
		32l * 1024l * 1024l };

TEST_METHOD(testLinearImageStreamContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file_1);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(container_1, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());
	std::shared_ptr<aff4::IAFF4Stream> stream = con->getImageStream(stream_1);
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		printf("  Read Size: %08" PRIu64 " : ", rSize);
		testStreamContents(stream, streamSHA1_1, rSize);
	}
}

TEST_METHOD(testAllocatedImageStreamContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file_2);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(container_2, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());
	std::shared_ptr<aff4::IAFF4Stream> stream = con->getImageStream(stream_2);
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		printf("  Read Size: %08" PRIu64 " : ", rSize);
		testStreamContents(stream, streamSHA1_2, rSize);
	}
}

TEST_METHOD(testReadErrorImageStreamContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file_3);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(container_3, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());
	std::shared_ptr<aff4::IAFF4Stream> stream = con->getImageStream(stream_3);
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		printf("  Read Size: %08" PRIu64 " : ", rSize);
		testStreamContents(stream, streamSHA1_3, rSize);
	}
}

TEST_METHOD(testAllHashsImageStreamContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file_4);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(container_4, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());
	std::shared_ptr<aff4::IAFF4Stream> stream = con->getImageStream(stream_4);
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		printf("  Read Size: %08" PRIu64 " : ", rSize);
		testStreamContents(stream, streamSHA1_4, rSize);
	}
}

TEST_METHOD(testMicro7ImageStreamContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file_5);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(container_5, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());
	std::shared_ptr<aff4::IAFF4Stream> stream = con->getImageStream(stream_5);
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		printf("  Read Size: %08" PRIu64 " : ", rSize);
		testStreamContents(stream, streamSHA1_5, rSize);
	}
}

TEST_METHOD(testMicro9ImageStreamContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file_6);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(container_6, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());
	std::shared_ptr<aff4::IAFF4Stream> stream = con->getImageStream(stream_6);
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		printf("  Read Size: %08" PRIu64 " : ", rSize);
		testStreamContents(stream, streamSHA1_6, rSize);
	}
}

#if defined _WIN32 && defined _MSC_VER 
	};
}
#endif
