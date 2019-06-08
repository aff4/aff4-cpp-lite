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
#include "container\AFF4ZipContainer.h"
#include "zip\Zip.h"
#include "TestUtilities.h"

#include <inttypes.h>

#define CPPUNIT_ASSERT Assert::IsTrue
#define CPPUNIT_ASSERT_EQUAL Assert::AreEqual

#define UNITTEST_BASE_PATH "..\\..\\..\\"

namespace AFF4UnitTests
{
	TEST_CLASS(TestContainer)
	{
	public:


#else

 /*
 * CPPUNIT
 */

#include "container.h"

CPPUNIT_TEST_SUITE_REGISTRATION(container);

container::container() {
}

container::~container() {
}

void container::setUp() {
}

void container::tearDown() {
}

#define UNITTEST_BASE_PATH

#define TEST_METHOD(x) void container::x()

#endif

const std::string filename = UNITTEST_BASE_PATH "tests/resources/Base-Linear.aff4";
const std::string resource = "aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044";
const std::string mapSHA1 = "5204743948cafe73b9c3d75052e52ead3d319cc7";
const std::string streamSHA1 = "ba85b601a65aef8adf7b0e0fb3144b217d4cd27c";
const std::string streamIndexSHA1 = "8bb7f7820cffbb3b14007e36d2b0ad2459c4d9fa";

const std::string filename1 = UNITTEST_BASE_PATH "tests/resources/blank.aff4";
const std::string filename2 = UNITTEST_BASE_PATH "tests/resources/blank5.aff4";

TEST_METHOD(testZipLinear) {
	std::string filename(UNITTEST_BASE_PATH "tests/resources/Base-Linear.aff4");
	aff4::zip::Zip container(filename);

	std::string expected("aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044");
	CPPUNIT_ASSERT(expected.compare(container.getZipComment()) == 0);

	// Check our entries.
	std::vector<std::shared_ptr<aff4::zip::ZipEntry>> entries = container.getEntries();
	CPPUNIT_ASSERT_EQUAL((size_t )10, entries.size());
	uint64_t offsets[] = { 71, 199, 367, 3048307, 3050390, 3052948, 3054527, 3169538, 3169821, 3169919 };
	uint64_t sizes[] = { 43, 36, 3047794, 1936, 2420, 1452, 114884, 152, 6, 6580 };
	int count = 0;
	int COUNT = 10;
	for (auto it = entries.begin(); it != entries.end(); it++) {
		std::shared_ptr<aff4::zip::ZipEntry> entry = *it;
		printf("%s %" PRIu64 ": %" PRIu64 "\n", entry->getSegmentName().c_str(), entry->getOffset(),
				entry->getLength());
		CPPUNIT_ASSERT(offsets[count] == entry->getOffset());
		CPPUNIT_ASSERT(sizes[count] == entry->getLength());
		count++;
		CPPUNIT_ASSERT(count <= COUNT);
	}
	CPPUNIT_ASSERT(count == COUNT);

	// Get a ZipSegmentStream and compare the results.
	std::string resource = "version.txt";
	std::shared_ptr<aff4::IAFF4Stream> stream = container.getStream(resource);
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL((uint64_t )36, stream->size());
	CPPUNIT_ASSERT(stream->getResourceID().compare(resource) == 0);

	std::unique_ptr<char[]> buffer(new char[stream->size()]);
	int64_t res = stream->read(buffer.get(), stream->size(), 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)36, (uint64_t)res);
	std::string version = std::string(buffer.get(), 36);
	printf("version.txt:\n%s\n", version.c_str());

	// Get a ZipSegmentStream and compare the results.
	resource = "aff4%3A%2F%2Ffcbfdce7-4488-4677-abf6-08bc931e195b/idx";
	stream = container.getStream(resource);
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL((uint64_t )152, stream->size());
	CPPUNIT_ASSERT(stream->getResourceID().compare(resource) == 0);

	std::unique_ptr<char[]> bufferIDX(new char[stream->size()]);
	res = stream->read(bufferIDX.get(), stream->size(), 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)152, (uint64_t)res);
	version = std::string(bufferIDX.get(), 152);
	printf("idx:\n%s\n", version.c_str());

	// And now hash a stream.
	resource = "aff4%3A%2F%2Fc215ba20-5648-4209-a793-1f918c723610/00000000";
	stream = container.getStream(resource);
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL((uint64_t )3047794, stream->size());
	CPPUNIT_ASSERT(stream->getResourceID().compare(resource) == 0);
	std::string sha1 = aff4::test::sha1sum(stream);
	expected = "ba85b601a65aef8adf7b0e0fb3144b217d4cd27c";
	printf("data: %s = %s\n", expected.c_str(), sha1.c_str());
	CPPUNIT_ASSERT(sha1.compare(expected) == 0);

	// And now hash a stream.
	resource = "information.turtle";
	stream = container.getStream(resource);
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL((uint64_t )6580, stream->size());
	CPPUNIT_ASSERT(stream->getResourceID().compare(resource) == 0);
	sha1 = aff4::test::sha1sum(stream);
	expected = "38cb789efd5e046c4a8bdbbaeb5c9227dcfc64b2";
	printf("turtle: %s = %s\n", expected.c_str(), sha1.c_str());
	CPPUNIT_ASSERT(sha1.compare(expected) == 0);

	container.close();
}

TEST_METHOD(testZipSegmentRead) {
	std::string filename(UNITTEST_BASE_PATH "tests/resources/Base-Linear.aff4");
	aff4::zip::Zip container(filename);

	std::string expected("aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044");
	std::string comment = container.getZipComment();
	int resstr = expected.compare(comment);
	CPPUNIT_ASSERT(resstr == 0);

	// Get a ZipSegmentStream and compare the results.
	std::string resource = "container.description";
	std::shared_ptr<aff4::IAFF4Stream> stream = container.getStream(resource);
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL((uint64_t )43, stream->size());
	CPPUNIT_ASSERT(stream->getResourceID().compare(resource) == 0);

	// Read full.
	std::unique_ptr<char[]> buffer(new char[stream->size()]);
	int64_t res = stream->read(buffer.get(), stream->size(), 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)43, (uint64_t)res);
	std::string description = std::string(buffer.get(), res);
	CPPUNIT_ASSERT(expected.compare(description) == 0);

	// Read partial
	res = stream->read(buffer.get(), 4, 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)4, (uint64_t)res);
	description = std::string(buffer.get(), res);
	CPPUNIT_ASSERT(std::string("aff4").compare(description) == 0);

	// Read partial
	res = stream->read(buffer.get(), 8, 7);
	CPPUNIT_ASSERT_EQUAL((uint64_t)8, (uint64_t)res);
	description = std::string(buffer.get(), res);
	CPPUNIT_ASSERT(std::string("685e15cc").compare(description) == 0);

	// End partial
	res = stream->read(buffer.get(), 4, 39);
	CPPUNIT_ASSERT_EQUAL((uint64_t)4, (uint64_t)res);
	description = std::string(buffer.get(), res);
	CPPUNIT_ASSERT(std::string("7044").compare(description) == 0);

	// clear buffer...
	::memset(buffer.get(), 0, 43);

	// Past end...
	res = stream->read(buffer.get(), 8, 39);
	CPPUNIT_ASSERT_EQUAL((uint64_t)4, (uint64_t)res);
	description = std::string(buffer.get(), res);
	CPPUNIT_ASSERT(std::string("7044").compare(description) == 0);

	// clear buffer...
	::memset(buffer.get(), 0, 43);

	// Past end...
	res = stream->read(buffer.get(), 8, 42);
	CPPUNIT_ASSERT_EQUAL((uint64_t)1, (uint64_t)res);
	description = std::string(buffer.get(), res);
	CPPUNIT_ASSERT(std::string("4").compare(description) == 0);

	// clear buffer...
	::memset(buffer.get(), 0, 43);

	// Past end...
	res = stream->read(buffer.get(), 8, 43);
	CPPUNIT_ASSERT_EQUAL((uint64_t)0, (uint64_t)res);

	// Past end...
	res = stream->read(buffer.get(), 8, 44);
	CPPUNIT_ASSERT_EQUAL((uint64_t)0, (uint64_t)res);

	container.close();
}

TEST_METHOD(testZipAllocated) {
	std::string filename(UNITTEST_BASE_PATH "tests/resources/Base-Allocated.aff4");
	aff4::zip::Zip container(filename);

	std::string expected("aff4://ce24a0d0-a540-442a-939e-938b848add9a");
	CPPUNIT_ASSERT(expected.compare(container.getZipComment()) == 0);

	// Check our entries.
	std::vector<std::shared_ptr<aff4::zip::ZipEntry>> entries = container.getEntries();
	CPPUNIT_ASSERT_EQUAL((size_t )10, entries.size());
	uint64_t offsets[] = { 71, 199, 367, 3048307, 3050390, 3052948, 3054527, 3066974, 3067292, 3068570 };
	uint64_t sizes[] = { 43, 36, 3047794, 1936, 2420, 1452, 12320, 187, 1186, 6583 };
	int count = 0;
	int COUNT = 10;
	for (auto it = entries.begin(); it != entries.end(); it++) {
		std::shared_ptr<aff4::zip::ZipEntry> entry = *it;
		printf("%s %" PRIu64 ": %" PRIu64 "\n", entry->getSegmentName().c_str(), entry->getOffset(),
				entry->getLength());
		CPPUNIT_ASSERT(offsets[count] == entry->getOffset());
		CPPUNIT_ASSERT(sizes[count] == entry->getLength());
		count++;
		CPPUNIT_ASSERT(count <= COUNT);
	}
	CPPUNIT_ASSERT(count == COUNT);

	// Get a ZipSegmentStream and compare the results.
	std::string resource = "version.txt";
	std::shared_ptr<aff4::IAFF4Stream> stream = container.getStream(resource);
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL((uint64_t )36, stream->size());
	CPPUNIT_ASSERT(stream->getResourceID().compare(resource) == 0);

	std::unique_ptr<char[]> buffer(new char[stream->size()]);
	int64_t res = stream->read(buffer.get(), stream->size(), 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)36, (uint64_t)res);
	std::string version = std::string(buffer.get(), 36);
	printf("version.txt:\n%s\n", version.c_str());

	// Get a ZipSegmentStream and compare the results.
	resource = "aff4%3A%2F%2Fe9cd53d3-b682-4f12-8045-86ba50a0239c/idx";
	stream = container.getStream(resource);
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL((uint64_t)187, (uint64_t)stream->size());
	CPPUNIT_ASSERT(stream->getResourceID().compare(resource) == 0);

	std::unique_ptr<char[]> bufferIDX(new char[stream->size()]);
	res = stream->read(bufferIDX.get(), stream->size(), 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)187, (uint64_t)res);
	version = std::string(bufferIDX.get(), 187);
	printf("idx:\n%s\n", version.c_str());

	// And now hash a stream.
	resource = "aff4%3A%2F%2Ffce3df71-dce8-4a17-af67-36bed58f25c9/00000000";
	stream = container.getStream(resource);
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL((uint64_t )3047794, stream->size());
	CPPUNIT_ASSERT(stream->getResourceID().compare(resource) == 0);
	std::string sha1 = aff4::test::sha1sum(stream);
	expected = "017f7093b97ef9349334c6654c678eb3c048b99b";
	printf("data: %s = %s\n", expected.c_str(), sha1.c_str());
	CPPUNIT_ASSERT(sha1.compare(expected) == 0);

	// And now hash a stream.
	resource = "information.turtle";
	stream = container.getStream(resource);
	CPPUNIT_ASSERT(stream != nullptr);
	CPPUNIT_ASSERT_EQUAL((uint64_t )6583, stream->size());
	CPPUNIT_ASSERT(stream->getResourceID().compare(resource) == 0);
	sha1 = aff4::test::sha1sum(stream);
	expected = "680b010ac80801cf9a555efe899d0ed2c3e6ecf6";
	printf("turtle: %s = %s\n", expected.c_str(), sha1.c_str());
	CPPUNIT_ASSERT(sha1.compare(expected) == 0);

	container.close();
}

TEST_METHOD(testContainerLinear) {
	std::string filename(UNITTEST_BASE_PATH "tests/resources/Base-Linear.aff4");

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044"), container->getResourceID());

	std::vector<aff4::rdf::RDFValue> tool = container->getProperty(aff4::Lexicon::AFF4_TOOL);
	CPPUNIT_ASSERT_EQUAL(1, (int )tool.size());
	CPPUNIT_ASSERT_EQUAL(std::string("Evimetry 2.2.0"), tool[0].toString());

	std::vector<aff4::rdf::RDFValue> major = container->getProperty(aff4::Lexicon::AFF4_MAJOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )major.size());
	CPPUNIT_ASSERT_EQUAL(std::string("1"), major[0].toString());

	std::vector<aff4::rdf::RDFValue> minor = container->getProperty(aff4::Lexicon::AFF4_MINOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )minor.size());
	CPPUNIT_ASSERT_EQUAL(std::string("0"), minor[0].toString());

	std::vector<aff4::rdf::RDFValue> rdfType = container->getProperty(aff4::Lexicon::AFF4_TYPE);
	CPPUNIT_ASSERT_EQUAL(1, (int )rdfType.size());
	CPPUNIT_ASSERT(aff4::Lexicon::AFF4_ZIP_TYPE == rdfType[0].getType());

	std::vector<aff4::rdf::RDFValue> time = container->getProperty(aff4::Lexicon::AFF4_CREATION_TIME);
	CPPUNIT_ASSERT_EQUAL(1, (int )time.size());
	CPPUNIT_ASSERT_EQUAL(std::string("2016-12-07T03:40:09Z"), time[0].toString());
	container->close();
}

TEST_METHOD(testContainerAllocated) {
	std::string filename(UNITTEST_BASE_PATH "tests/resources/Base-Allocated.aff4");

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://ce24a0d0-a540-442a-939e-938b848add9a"), container->getResourceID());

	std::vector<aff4::rdf::RDFValue> tool = container->getProperty(aff4::Lexicon::AFF4_TOOL);
	CPPUNIT_ASSERT_EQUAL(1, (int )tool.size());
	CPPUNIT_ASSERT_EQUAL(std::string("Evimetry 2.2.0"), tool[0].toString());

	std::vector<aff4::rdf::RDFValue> major = container->getProperty(aff4::Lexicon::AFF4_MAJOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )major.size());
	CPPUNIT_ASSERT_EQUAL(std::string("1"), major[0].toString());

	std::vector<aff4::rdf::RDFValue> minor = container->getProperty(aff4::Lexicon::AFF4_MINOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )minor.size());
	CPPUNIT_ASSERT_EQUAL(std::string("0"), minor[0].toString());

	std::vector<aff4::rdf::RDFValue> rdfType = container->getProperty(aff4::Lexicon::AFF4_TYPE);
	CPPUNIT_ASSERT_EQUAL(1, (int )rdfType.size());
	CPPUNIT_ASSERT_EQUAL(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_ZIP_TYPE), rdfType[0].toString());

	std::vector<aff4::rdf::RDFValue> time = container->getProperty(aff4::Lexicon::AFF4_CREATION_TIME);
	CPPUNIT_ASSERT_EQUAL(1, (int )time.size());
	CPPUNIT_ASSERT_EQUAL(std::string("2016-12-07T03:40:07Z"), time[0].toString());
	container->close();
}

TEST_METHOD(testContainerLinearReadError) {
	std::string filename(UNITTEST_BASE_PATH "tests/resources/Base-Linear-ReadError.aff4");

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://686e3512-b568-48fd-ac7b-73764b98a9aa"), container->getResourceID());

	std::vector<aff4::rdf::RDFValue> tool = container->getProperty(aff4::Lexicon::AFF4_TOOL);
	CPPUNIT_ASSERT_EQUAL(1, (int )tool.size());
	CPPUNIT_ASSERT_EQUAL(std::string("Evimetry 2.2.0"), tool[0].toString());

	std::vector<aff4::rdf::RDFValue> major = container->getProperty(aff4::Lexicon::AFF4_MAJOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )major.size());
	CPPUNIT_ASSERT_EQUAL(std::string("1"), major[0].toString());

	std::vector<aff4::rdf::RDFValue> minor = container->getProperty(aff4::Lexicon::AFF4_MINOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )minor.size());
	CPPUNIT_ASSERT_EQUAL(std::string("0"), minor[0].toString());

	std::vector<aff4::rdf::RDFValue> rdfType = container->getProperty(aff4::Lexicon::AFF4_TYPE);
	CPPUNIT_ASSERT_EQUAL(1, (int )rdfType.size());
	CPPUNIT_ASSERT_EQUAL(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_ZIP_TYPE), rdfType[0].toString());

	std::vector<aff4::rdf::RDFValue> time = container->getProperty(aff4::Lexicon::AFF4_CREATION_TIME);
	CPPUNIT_ASSERT_EQUAL(1, (int )time.size());
	CPPUNIT_ASSERT_EQUAL(std::string("2016-12-07T03:40:10Z"), time[0].toString());
	container->close();
}

TEST_METHOD(testContainerLinearAllHashes) {
	std::string filename(UNITTEST_BASE_PATH "tests/resources/Base-Linear-AllHashes.aff4");

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://7a86cb01-217c-4852-b8e0-c94be1ca5ac5"), container->getResourceID());

	std::vector<aff4::rdf::RDFValue> tool = container->getProperty(aff4::Lexicon::AFF4_TOOL);
	CPPUNIT_ASSERT_EQUAL(1, (int )tool.size());
	CPPUNIT_ASSERT_EQUAL(std::string("Evimetry 3.0.0"), tool[0].toString());

	std::vector<aff4::rdf::RDFValue> major = container->getProperty(aff4::Lexicon::AFF4_MAJOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )major.size());
	CPPUNIT_ASSERT_EQUAL(std::string("1"), major[0].toString());

	std::vector<aff4::rdf::RDFValue> minor = container->getProperty(aff4::Lexicon::AFF4_MINOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )minor.size());
	CPPUNIT_ASSERT_EQUAL(std::string("0"), minor[0].toString());

	std::vector<aff4::rdf::RDFValue> rdfType = container->getProperty(aff4::Lexicon::AFF4_TYPE);
	CPPUNIT_ASSERT_EQUAL(1, (int )rdfType.size());
	CPPUNIT_ASSERT_EQUAL(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_ZIP_TYPE), rdfType[0].toString());

	std::vector<aff4::rdf::RDFValue> time = container->getProperty(aff4::Lexicon::AFF4_CREATION_TIME);
	CPPUNIT_ASSERT_EQUAL(1, (int )time.size());
	CPPUNIT_ASSERT_EQUAL(std::string("2017-03-09T03:51:42Z"), time[0].toString());
	container->close();

}

void testContainerDescriptionInt(std::shared_ptr<aff4::IAFF4Container> container,
	std::shared_ptr<aff4::IAFF4Stream> stream) {
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT(stream != nullptr);
	std::unique_ptr<char[]> buffer(new char[stream->size()]);
	int64_t res = stream->read(buffer.get(), stream->size(), 0);
	CPPUNIT_ASSERT_EQUAL((uint64_t)res, stream->size());
	std::string description(buffer.get(), stream->size());
	CPPUNIT_ASSERT_EQUAL(container->getResourceID(), description);
}

void testStreamContents(std::shared_ptr<aff4::IAFF4Stream> stream, std::string expectedSHA1) {
	CPPUNIT_ASSERT(stream != nullptr);
	std::string sha1 = aff4::test::sha1sum(stream);
	printf("%s: %s = %s\n", stream->getResourceID().c_str(), expectedSHA1.c_str(), sha1.c_str());
	CPPUNIT_ASSERT(sha1.compare(expectedSHA1) == 0);
}

TEST_METHOD(testContainerDescription) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(resource, container->getResourceID());
	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());

	// Get the base container.description file
	std::string res = "container.description";
	testContainerDescriptionInt(container, con->getSegment(res));
	// Get the base container.description file
	res = "/container.description";
	testContainerDescriptionInt(container, con->getSegment(res));
	res = "//container.description";
	testContainerDescriptionInt(container, con->getSegment(res));
	res = "///container.description";
	testContainerDescriptionInt(container, con->getSegment(res));
	res = "////container.description";
	testContainerDescriptionInt(container, con->getSegment(res));
	// Get the base container.description file, but prefixed with the containers resource URL.
	res = "aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044/container.description";
	testContainerDescriptionInt(container, con->getSegment(res));
}

TEST_METHOD(testContainerMissingResource) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(resource, container->getResourceID());
	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());

	std::string res = "container.description2";
	std::shared_ptr<aff4::IAFF4Stream> s = con->getSegment(res);
	CPPUNIT_ASSERT(s == nullptr);

	res = "aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044";
	s = con->getSegment(res);
	CPPUNIT_ASSERT(s == nullptr);

	res = "aff4://c215ba20-5648-4209-a793-1f918c723610";
	s = con->getSegment(res);
	CPPUNIT_ASSERT(s == nullptr);
}

TEST_METHOD(testContainerMapContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(resource, container->getResourceID());
	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());

	// Get the base container.description file
	std::string res = "aff4://fcbfdce7-4488-4677-abf6-08bc931e195b/map";
	testStreamContents(con->getSegment(res), mapSHA1);

	res = "/aff4://fcbfdce7-4488-4677-abf6-08bc931e195b/map";
	testStreamContents(con->getSegment(res), mapSHA1);

	res = "aff4%3A%2F%2Ffcbfdce7-4488-4677-abf6-08bc931e195b/map";
	testStreamContents(con->getSegment(res), mapSHA1);

	res = "/aff4%3A%2F%2Ffcbfdce7-4488-4677-abf6-08bc931e195b/map";
	testStreamContents(con->getSegment(res), mapSHA1);
}

TEST_METHOD(testContainerImageStreamContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(resource, container->getResourceID());
	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());

	// Get the base container.description file
	std::string res = "aff4://c215ba20-5648-4209-a793-1f918c723610/00000000.index";
	testStreamContents(con->getSegment(res), streamIndexSHA1);

	res = "aff4://c215ba20-5648-4209-a793-1f918c723610/00000000";
	testStreamContents(con->getSegment(res), streamSHA1);
}

TEST_METHOD(testBlank) {
	std::string filename(filename1);

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://b437c880-9f5a-420e-8553-8878f5518441"), container->getResourceID());

	std::vector<aff4::rdf::RDFValue> tool = container->getProperty(aff4::Lexicon::AFF4_TOOL);
	CPPUNIT_ASSERT_EQUAL(1, (int )tool.size());
	CPPUNIT_ASSERT_EQUAL(std::string("libaff4 1.0"), tool[0].toString());

	std::vector<aff4::rdf::RDFValue> major = container->getProperty(aff4::Lexicon::AFF4_MAJOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )major.size());
	CPPUNIT_ASSERT_EQUAL(std::string("1"), major[0].toString());

	std::vector<aff4::rdf::RDFValue> minor = container->getProperty(aff4::Lexicon::AFF4_MINOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )minor.size());
	CPPUNIT_ASSERT_EQUAL(std::string("0"), minor[0].toString());

	std::vector<aff4::rdf::RDFValue> rdfType = container->getProperty(aff4::Lexicon::AFF4_TYPE);
	CPPUNIT_ASSERT_EQUAL(1, (int )rdfType.size());
	CPPUNIT_ASSERT(aff4::Lexicon::AFF4_ZIP_TYPE == rdfType[0].getType());

	std::vector<aff4::rdf::RDFValue> time = container->getProperty(aff4::Lexicon::AFF4_CREATION_TIME);
	CPPUNIT_ASSERT_EQUAL(0, (int )time.size());
	container->close();
}

TEST_METHOD(testBlank5) {
	std::string filename(filename2);

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://37de92d3-24bb-4e5f-8279-a2b3992eba52"), container->getResourceID());

	std::vector<aff4::rdf::RDFValue> tool = container->getProperty(aff4::Lexicon::AFF4_TOOL);
	CPPUNIT_ASSERT_EQUAL(1, (int )tool.size());
	CPPUNIT_ASSERT_EQUAL(std::string("libaff4 1.0"), tool[0].toString());

	std::vector<aff4::rdf::RDFValue> major = container->getProperty(aff4::Lexicon::AFF4_MAJOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )major.size());
	CPPUNIT_ASSERT_EQUAL(std::string("1"), major[0].toString());

	std::vector<aff4::rdf::RDFValue> minor = container->getProperty(aff4::Lexicon::AFF4_MINOR_VERSION);
	CPPUNIT_ASSERT_EQUAL(1, (int )minor.size());
	CPPUNIT_ASSERT_EQUAL(std::string("0"), minor[0].toString());

	std::vector<aff4::rdf::RDFValue> rdfType = container->getProperty(aff4::Lexicon::AFF4_TYPE);
	CPPUNIT_ASSERT_EQUAL(1, (int )rdfType.size());
	CPPUNIT_ASSERT(aff4::Lexicon::AFF4_ZIP_TYPE == rdfType[0].getType());

	std::vector<aff4::rdf::RDFValue> time = container->getProperty(aff4::Lexicon::AFF4_CREATION_TIME);
	CPPUNIT_ASSERT_EQUAL(0, (int )time.size());
	container->close();
}


#if defined _WIN32 && defined _MSC_VER 

	};

}

#endif
