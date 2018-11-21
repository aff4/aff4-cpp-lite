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
#include"container\AFF4ZipContainer.h"
#include "stream\ImageStreamFactory.h"

#include <locale>
#include <codecvt>

#define CPPUNIT_ASSERT Assert::IsTrue
#define CPPUNIT_ASSERT_EQUAL Assert::AreEqual

#define UNITTEST_BASE_PATH "..\\..\\..\\"

namespace AFF4UnitTests
{
	TEST_CLASS(TestImages)
	{
	public:


#else

 /*
 * CPPUNIT
 */

#include "image.h"
#include <locale>
#include <codecvt>

CPPUNIT_TEST_SUITE_REGISTRATION(image);

image::image() {
}

image::~image() {
}

void image::setUp() {
}

void image::tearDown() {
}

#define TEST_METHOD(x) void image::x()
#define UNITTEST_BASE_PATH

#endif

#ifdef _WIN32
#define UNITTEST_PATH "\\"
#else
#define UNITTEST_PATH "/"
#endif

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

const std::string file_1 = UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Base-Linear.aff4";
const std::string container_1 = "aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044";
const std::string image_1 = "aff4://cf853d0b-5589-4c7c-8358-2ca1572b87eb";
const std::string map_1 = "aff4://fcbfdce7-4488-4677-abf6-08bc931e195b";
const uint64_t mapSize_1 = 268435456;
const std::string streamSHA1_1 = "7d3d27f667f95f7ec5b9d32121622c0f4b60b48d";

const std::string file_2 = UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Base-Allocated.aff4";
const std::string container_2 = "aff4://ce24a0d0-a540-442a-939e-938b848add9a";
const std::string image_2 = "aff4://8fcced2b-989f-4f51-bfa2-38d4a4d818fe";
const std::string map_2 = "aff4://e9cd53d3-b682-4f12-8045-86ba50a0239c";
const uint64_t mapSize_2 = 268435456;
const std::string streamSHA1_2 = "e8650e89b262cf0b4b73c025312488d5a6317a26";

const std::string file_3 = UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Base-Linear-ReadError.aff4";
const std::string container_3 = "aff4://686e3512-b568-48fd-ac7b-73764b98a9aa";
const std::string image_3 = "aff4://3a873665-7bf6-47b5-a12a-d6632a58ddf9";
const std::string map_3 = "aff4://b282d5f4-333a-4f6a-b96f-0e5138bb18c8";
const uint64_t mapSize_3 = 268435456;
const std::string streamSHA1_3 = "67e245a640e2784ead30c1ff1a3f8d237b58310f";

const std::string file_4 = UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Base-Linear-AllHashes.aff4";
const std::string container_4 = "aff4://7a86cb01-217c-4852-b8e0-c94be1ca5ac5";
const std::string image_4 = "aff4://e8733831-f8fc-4573-87d7-beb7fe708e96";
const std::string map_4 = "aff4://2a497fe5-0221-4156-8b4d-176bebf7163f";
const uint64_t mapSize_4 = 268435456;
const std::string streamSHA1_4 = "7d3d27f667f95f7ec5b9d32121622c0f4b60b48d";

const std::string file_5 = UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Base-Allocated.aff4";
const std::string container_5 = "aff4://ce24a0d0-a540-442a-939e-938b848add9a";
const std::string image_5 = "aff4://8fcced2b-989f-4f51-bfa2-38d4a4d818fe";
const std::string map_5 = "aff4://e9cd53d3-b682-4f12-8045-86ba50a0239c";
const uint64_t mapSize_5 = 268435456;
const std::string streamSHA1_5 = "7d3d27f667f95f7ec5b9d32121622c0f4b60b48d";

const std::string pfile_1 = UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Micro7.001.aff4";
const std::string pcontainer_1 = "aff4://2ec3bb84-c9fb-4c4e-beba-add19fe60b99";
const std::string pimage_1 = "aff4://1df01c49-a36a-4770-b6e4-1ef4efe0f4b7";
const std::string pmap_1 = "aff4://02928200-4eef-4e8b-b2fb-e8b36f368063";
const uint64_t pmapSize_1 = 262144;
const std::string pstreamSHA1_1 = "deaa3877443908ac4d01ca410ff1012c8ab0db35";

const std::string pfile_2 = UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Micro9.001.aff4";
const std::string pcontainer_2 = "aff4://80b64e67-39f1-4564-948a-911f06f6343f";
const std::string pimage_2 = "aff4://6ead3312-8e9e-4dd8-931c-9d91c2dba63c";
const std::string pmap_2 = "aff4://7101b94e-2cde-4ecd-b177-8f542644091d";
const uint64_t pmapSize_2 = 73728;
const std::string pstreamSHA1_2 = "2b0496d2ec4189f8190a8c3f8bf92f503706cf96";

const std::string strip_file_1 = UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Striped" UNITTEST_PATH "Base-Linear_1.aff4";
const std::string strip_container_1 = "aff4://7cbb47d0-b04c-42bc-8c04-87b7782739ad";
const std::string strip_image_1 = "aff4://951b3e29-6549-4266-8e81-3f88ddba61ae";
const std::string strip_map_1 = "aff4://2dd04819-73c8-40e3-a32b-fdddb0317eac";
const uint64_t strip_mapSize_1 = 268435456;
const std::string strip_streamSHA1_1 = "7d3d27f667f95f7ec5b9d32121622c0f4b60b48d";

const std::string strip_file_2 = UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Striped" UNITTEST_PATH "Base-Linear_2.aff4";
const std::string strip_container_2 = "aff4://51725cd9-3769-4be7-a8ab-94e3ea62bf9a";
const std::string strip_image_2 = "aff4://951b3e29-6549-4266-8e81-3f88ddba61ae";
const std::string strip_map_2 = "aff4://363ac10c-8d8d-4905-ac25-a14aaddd8a41";
const uint64_t strip_mapSize_2 = 268435456;
const std::string strip_streamSHA1_2 = "7d3d27f667f95f7ec5b9d32121622c0f4b60b48d";

void testStreamContentsInt(std::shared_ptr<aff4::IAFF4Stream> stream, std::string expectedSHA1, uint64_t readSize) {
	printf("  Read Size: %08" PRIu64 " : ", readSize);
	CPPUNIT_ASSERT(stream != nullptr);
	std::string sha1 = aff4::test::sha1sum(stream, readSize);
	printf("%s: %s = %s\n", stream->getResourceID().c_str(), expectedSHA1.c_str(), sha1.c_str());
	CPPUNIT_ASSERT_EQUAL(expectedSHA1, sha1);
}

TEST_METHOD(testContainerLinear) {
	std::string filename(UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Base-Linear.aff4");

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://685e15cc-d0fb-4dbc-ba47-48117fc77044"), container->getResourceID());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = container->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	aff4::test::printResourceProperties(image);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://cf853d0b-5589-4c7c-8358-2ca1572b87eb"), image->getResourceID());

	aff4::test::testProperty(image, aff4::Lexicon::AFF4_BLOCKSIZE, 512);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_SECTOR_COUNT, (int64_t) 524288);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_NAME, "/dev/sdz");
	aff4::test::testPropertyResource(image, aff4::Lexicon::AFF4_DATASTREAM,
			"aff4://fcbfdce7-4488-4677-abf6-08bc931e195b");
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_SIZE, (int64_t) 268435456);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_SERIAL, "SGAT5060001234");
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_CASE_DESCRIPTION, "Canonical Image Generation Test Case");

	container->close();
}

TEST_METHOD(testContainerAllocated) {
	std::string filename(UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Base-Allocated.aff4");

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://ce24a0d0-a540-442a-939e-938b848add9a"), container->getResourceID());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = container->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	aff4::test::printResourceProperties(image);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://8fcced2b-989f-4f51-bfa2-38d4a4d818fe"), image->getResourceID());

	aff4::test::testProperty(image, aff4::Lexicon::AFF4_BLOCKSIZE, 512);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_SECTOR_COUNT, (int64_t) 524288);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_NAME, "/dev/sdz");
	aff4::test::testPropertyResource(image, aff4::Lexicon::AFF4_DATASTREAM,
			"aff4://e9cd53d3-b682-4f12-8045-86ba50a0239c");
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_SIZE, (int64_t) 268435456);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_SERIAL, "SGAT5060001234");
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_CASE_DESCRIPTION, "Canonical Image Generation Test Case");

	container->close();
}

TEST_METHOD(testContainerLinearReadError) {
	std::string filename(UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Base-Linear-ReadError.aff4");

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://686e3512-b568-48fd-ac7b-73764b98a9aa"), container->getResourceID());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = container->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	aff4::test::printResourceProperties(image);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://3a873665-7bf6-47b5-a12a-d6632a58ddf9"), image->getResourceID());

	aff4::test::testProperty(image, aff4::Lexicon::AFF4_BLOCKSIZE, 512);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_SECTOR_COUNT, (int64_t) 524288);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_NAME, "/dev/sdz");
	aff4::test::testPropertyResource(image, aff4::Lexicon::AFF4_DATASTREAM,
			"aff4://b282d5f4-333a-4f6a-b96f-0e5138bb18c8");
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_SIZE, (int64_t) 268435456);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_SERIAL, "SGAT5060001234");
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_CASE_DESCRIPTION, "Canonical Image Generation Test Case");

	container->close();
}

TEST_METHOD(testContainerLinearAllHashes) {
	std::string filename(UNITTEST_BASE_PATH "tests" UNITTEST_PATH "resources" UNITTEST_PATH "Base-Linear-AllHashes.aff4");

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(filename);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://7a86cb01-217c-4852-b8e0-c94be1ca5ac5"), container->getResourceID());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = container->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	aff4::test::printResourceProperties(image);
	CPPUNIT_ASSERT_EQUAL(std::string("aff4://e8733831-f8fc-4573-87d7-beb7fe708e96"), image->getResourceID());

	aff4::test::testProperty(image, aff4::Lexicon::AFF4_BLOCKSIZE, 512);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_SECTOR_COUNT, (int64_t) 524288);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_NAME, "/dev/sdz");
	aff4::test::testPropertyResource(image, aff4::Lexicon::AFF4_DATASTREAM,
			"aff4://2a497fe5-0221-4156-8b4d-176bebf7163f");
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_SIZE, (int64_t) 268435456);
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_DISK_DEVICE_SERIAL, "SGAT5060001234");
	aff4::test::testProperty(image, aff4::Lexicon::AFF4_CASE_DESCRIPTION, "Canonical Image Generation Test Case");

	container->close();

}


TEST_METHOD(testLinearImageStreamContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file_1);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(container_1, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = con->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	CPPUNIT_ASSERT_EQUAL(image_1, image->getResourceID());

	std::shared_ptr<aff4::IAFF4Map> map = image->getMap();
	CPPUNIT_ASSERT(map != nullptr);
	CPPUNIT_ASSERT_EQUAL(map_1, map->getResourceID());
	CPPUNIT_ASSERT_EQUAL(mapSize_1, map->size());

	std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", map->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		testStreamContentsInt(stream, streamSHA1_1, rSize);
	}
}

TEST_METHOD(testAllocatedImageStreamContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file_2);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(container_2, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = con->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	CPPUNIT_ASSERT_EQUAL(image_2, image->getResourceID());

	std::shared_ptr<aff4::IAFF4Map> map = image->getMap();
	CPPUNIT_ASSERT(map != nullptr);
	CPPUNIT_ASSERT_EQUAL(map_2, map->getResourceID());
	CPPUNIT_ASSERT_EQUAL(mapSize_2, map->size());

	std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		testStreamContentsInt(stream, streamSHA1_2, rSize);
	}
}

TEST_METHOD(testReadErrorImageStreamContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file_3);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(container_3, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = con->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	CPPUNIT_ASSERT_EQUAL(image_3, image->getResourceID());

	std::shared_ptr<aff4::IAFF4Map> map = image->getMap();
	CPPUNIT_ASSERT(map != nullptr);
	CPPUNIT_ASSERT_EQUAL(map_3, map->getResourceID());
	CPPUNIT_ASSERT_EQUAL(mapSize_3, map->size());

	std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		testStreamContentsInt(stream, streamSHA1_3, rSize);
	}
}

TEST_METHOD(testAllHashsImageStreamContents) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file_4);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(container_4, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = con->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	CPPUNIT_ASSERT_EQUAL(image_4, image->getResourceID());

	std::shared_ptr<aff4::IAFF4Map> map = image->getMap();
	CPPUNIT_ASSERT(map != nullptr);
	CPPUNIT_ASSERT_EQUAL(map_4, map->getResourceID());
	CPPUNIT_ASSERT_EQUAL(mapSize_4, map->size());

	std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		testStreamContentsInt(stream, streamSHA1_4, rSize);
	}
}

TEST_METHOD(testContainerAllocatedUnknown) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file_5);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(container_5, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = con->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	CPPUNIT_ASSERT_EQUAL(image_5, image->getResourceID());

	std::shared_ptr<aff4::IAFF4Map> map = image->getMap();
	CPPUNIT_ASSERT(map != nullptr);
	CPPUNIT_ASSERT_EQUAL(map_5, map->getResourceID());
	CPPUNIT_ASSERT_EQUAL(mapSize_5, map->size());

	// Override the Unknown Stream BEFORE we get our stream instance.
	std::shared_ptr<aff4::IAFF4Stream> zero = aff4::stream::createZeroStream();
	map->setUnknownStreamOverride(zero);

	std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		testStreamContentsInt(stream, streamSHA1_5, rSize);
	}
}

/*
 * Physical Memory images.
 */



TEST_METHOD(testContainer7) {

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(pfile_1);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(pcontainer_1, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = con->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	CPPUNIT_ASSERT_EQUAL(pimage_1, image->getResourceID());

	std::shared_ptr<aff4::IAFF4Map> map = image->getMap();
	CPPUNIT_ASSERT(map != nullptr);
	CPPUNIT_ASSERT_EQUAL(pmap_1, map->getResourceID());
	CPPUNIT_ASSERT_EQUAL(pmapSize_1, map->size());

	std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", map->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		testStreamContentsInt(stream, pstreamSHA1_1, rSize);
	}
}

TEST_METHOD(testContainer9) {
	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(pfile_2);
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(pcontainer_2, container->getResourceID());

	aff4::container::AFF4ZipContainer* con = static_cast<aff4::container::AFF4ZipContainer*>(container.get());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = con->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	CPPUNIT_ASSERT_EQUAL(pimage_2, image->getResourceID());

	std::shared_ptr<aff4::IAFF4Map> map = image->getMap();
	CPPUNIT_ASSERT(map != nullptr);
	CPPUNIT_ASSERT_EQUAL(pmap_2, map->getResourceID());
	CPPUNIT_ASSERT_EQUAL(pmapSize_2, map->size());

	std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		testStreamContentsInt(stream, pstreamSHA1_2, rSize);
	}
}

/*
 * Physical Memory images.
 */



TEST_METHOD(testContainerLinear_Part1) {
	std::unique_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(strip_file_1));

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(strip_file_1, resolver.get());
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(strip_container_1, container->getResourceID());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = container->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	CPPUNIT_ASSERT_EQUAL(strip_image_1, image->getResourceID());

	std::shared_ptr<aff4::IAFF4Map> map = image->getMap();
	CPPUNIT_ASSERT(map != nullptr);
	CPPUNIT_ASSERT_EQUAL(strip_map_1, map->getResourceID());
	CPPUNIT_ASSERT_EQUAL(strip_mapSize_1, map->size());

	std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		testStreamContentsInt(stream, strip_streamSHA1_1, rSize);
	}
}

TEST_METHOD(testContainerLinear_Part2) {
	std::unique_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(strip_file_2));

	std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(strip_file_2, resolver.get());
	CPPUNIT_ASSERT(container != nullptr);
	CPPUNIT_ASSERT_EQUAL(strip_container_2, container->getResourceID());

	std::vector<std::shared_ptr<aff4::IAFF4Image>> images = container->getImages();
	CPPUNIT_ASSERT_EQUAL(1, (int )images.size());

	std::shared_ptr<aff4::IAFF4Image> image = images[0];
	CPPUNIT_ASSERT_EQUAL(strip_image_2, image->getResourceID());

	std::shared_ptr<aff4::IAFF4Map> map = image->getMap();
	CPPUNIT_ASSERT(map != nullptr);
	CPPUNIT_ASSERT_EQUAL(strip_map_2, map->getResourceID());
	CPPUNIT_ASSERT_EQUAL(strip_mapSize_2, map->size());

	std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
	CPPUNIT_ASSERT(stream != nullptr);
	printf("Stream : %s\n", stream->getResourceID().c_str());
	for (uint64_t rSize : readSizes) {
		testStreamContentsInt(stream, strip_streamSHA1_2, rSize);
	}
}

TEST_METHOD(testCAPI_Linear) {
	AFF4_init();
	int handle = AFF4_open(file_1.c_str());
	CPPUNIT_ASSERT_EQUAL(1, handle);
	uint64_t size = AFF4_object_size(handle);
	CPPUNIT_ASSERT_EQUAL(mapSize_1, size);

	// Create a digest of this
	std::string sha1 = aff4::test::sha1sum(handle, size);
	CPPUNIT_ASSERT_EQUAL(streamSHA1_1, sha1);

	// Close the container.
	AFF4_close(handle);

	// See it still exists.
	CPPUNIT_ASSERT_EQUAL(-1, AFF4_close(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_object_size(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_read(handle, 0, nullptr, 1));
}

TEST_METHOD(testCAPI_Allocated) {
	AFF4_init();
	int handle = AFF4_open(file_2.c_str());
	CPPUNIT_ASSERT_EQUAL(1, handle);
	uint64_t size = AFF4_object_size(handle);
	CPPUNIT_ASSERT_EQUAL(mapSize_2, size);

	// Create a digest of this
	std::string sha1 = aff4::test::sha1sum(handle, size);
	CPPUNIT_ASSERT_EQUAL(streamSHA1_2, sha1);

	// Close the container.
	AFF4_close(handle);

	// See it still exists.
	CPPUNIT_ASSERT_EQUAL(-1, AFF4_close(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_object_size(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_read(handle, 0, nullptr, 1));
}

TEST_METHOD(testCAPI_Striped) {
	AFF4_init();
	int handle = AFF4_open(strip_file_1.c_str());
	CPPUNIT_ASSERT_EQUAL(1, handle);
	uint64_t size = AFF4_object_size(handle);
	CPPUNIT_ASSERT_EQUAL(strip_mapSize_1, size);

	// Create a digest of this
	std::string sha1 = aff4::test::sha1sum(handle, size);
	CPPUNIT_ASSERT_EQUAL(strip_streamSHA1_1, sha1);

	// Close the container.
	AFF4_close(handle);

	// See it still exists.
	CPPUNIT_ASSERT_EQUAL(-1, AFF4_close(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_object_size(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_read(handle, 0, nullptr, 1));
}

TEST_METHOD(testCAPI_PMem) {
	AFF4_init();
	int handle = AFF4_open(pfile_1.c_str());
	CPPUNIT_ASSERT_EQUAL(1, handle);
	uint64_t size = AFF4_object_size(handle);
	CPPUNIT_ASSERT_EQUAL(pmapSize_1, size);

	// Create a digest of this
	std::string sha1 = aff4::test::sha1sum(handle, size);
	CPPUNIT_ASSERT_EQUAL(pstreamSHA1_1, sha1);

	// Close the container.
	AFF4_close(handle);

	// See it still exists.
	CPPUNIT_ASSERT_EQUAL(-1, AFF4_close(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_object_size(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_read(handle, 0, nullptr, 1));
}

std::wstring s2ws(const std::string& str) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.from_bytes(str);
}

std::string ws2s(const std::wstring& wstr) {
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;
	return converterX.to_bytes(wstr);
}


TEST_METHOD(testUnicodeFilename) {
	// UTF-8 encoded string.
	std::string file("D:\\сховище\\1GBFAT16Default.aff4");
	AFF4_init();
	int handle = AFF4_open(file.c_str());
	CPPUNIT_ASSERT_EQUAL(1, handle);
	uint64_t size = AFF4_object_size(handle);
	uint64_t s = 1073741824;
	CPPUNIT_ASSERT_EQUAL(s, size);

	// Close the container.
	AFF4_close(handle);

	// See it still exists.
	CPPUNIT_ASSERT_EQUAL(-1, AFF4_close(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_object_size(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_read(handle, 0, nullptr, 1));
}

TEST_METHOD(testUnicodeFilename2) {
	// UTF-8 encoded string.
	std::string file("D:\\сховище\\справа.aff4");
	AFF4_init();
	int handle = AFF4_open(file.c_str());
	CPPUNIT_ASSERT_EQUAL(1, handle);
	uint64_t size = AFF4_object_size(handle);
	uint64_t s = 268435456;
	CPPUNIT_ASSERT_EQUAL(s, size);

	// Close the container.
	AFF4_close(handle);

	// See it still exists.
	CPPUNIT_ASSERT_EQUAL(-1, AFF4_close(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_object_size(handle));
	CPPUNIT_ASSERT_EQUAL(-1, (int)AFF4_read(handle, 0, nullptr, 1));
}


#if defined _WIN32 && defined _MSC_VER 

	};

}

#endif
