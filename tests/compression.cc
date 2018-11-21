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
#include "codec\NullCompression.h"
#include "codec\SnappyCompression.h"
#include "codec\DeflateCompression.h"
#include "codec\LZ4Compression.h"
#include "codec\CompressionCodec.h"
#include <functional>
#include <zlib.h>
#include <snappy.h>
#include <lz4.h>

#define CPPUNIT_ASSERT Assert::IsTrue
#define CPPUNIT_ASSERT_EQUAL Assert::AreEqual

static std::string srcText =
"alksjdfwienflkdfasdfasfasdfasdfasdfadfasdflka jflaskjadflk;jas ;lkdfjlaskdfjlaskdjflkalksjdfwienflkdfasdfasfasdfasdfasdalksjdfwi";
static int blockLength = srcText.length();

namespace AFF4UnitTests
{
	TEST_CLASS(TestCompression)
	{
	public:


#else

 /*
 * CPPUNIT
 */

#include "compression.h"

static std::string srcText =
"alksjdfwienflkdfasdfasfasdfasdfasdfadfasdflka jflaskjadflk;jas ;lkdfjlaskdfjlaskdjflkalksjdfwienflkdfasdfasfasdfasdfasdalksjdfwi";
static int blockLength = srcText.length();

CPPUNIT_TEST_SUITE_REGISTRATION(compression);

compression::compression() {
}

compression::~compression() {
}

void compression::setUp() {
}

void compression::tearDown() {
}

#define TEST_METHOD(x) void compression::x()

#endif

TEST_METHOD(testSnappy) {
	std::string res = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGE_COMPRESSION_SNAPPY);
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(res, AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec != nullptr);
	CPPUNIT_ASSERT_EQUAL(res, codec->getResourceID());
	CPPUNIT_ASSERT_EQUAL((uint32_t)AFF4_DEFAULT_CHUNK_SIZE, codec->getChunkSize());

	// Ensure we get the correct derived type.
	aff4::codec::CompressionCodec* codecPtr = codec.get();
	aff4::codec::SnappyCompression* derived = dynamic_cast<aff4::codec::SnappyCompression*>(codecPtr);
	CPPUNIT_ASSERT(derived != nullptr);
}

TEST_METHOD(testSnappyResource) {
	std::string res = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGE_COMPRESSION_SNAPPY);
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(
			aff4::Lexicon::AFF4_IMAGE_COMPRESSION_SNAPPY, AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec != nullptr);
	CPPUNIT_ASSERT_EQUAL(res, codec->getResourceID());
	CPPUNIT_ASSERT_EQUAL((uint32_t)AFF4_DEFAULT_CHUNK_SIZE, codec->getChunkSize());

	// Ensure we get the correct derived type.
	aff4::codec::CompressionCodec* codecPtr = codec.get();
	aff4::codec::SnappyCompression* derived = dynamic_cast<aff4::codec::SnappyCompression*>(codecPtr);
	CPPUNIT_ASSERT(derived != nullptr);
}

TEST_METHOD(testLZ4) {
	std::string res = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGE_COMPRESSION_LZ4);
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(res, AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec != nullptr);
	CPPUNIT_ASSERT_EQUAL(res, codec->getResourceID());
	CPPUNIT_ASSERT_EQUAL((uint32_t)AFF4_DEFAULT_CHUNK_SIZE, codec->getChunkSize());

	// Ensure we get the correct derived type.
	aff4::codec::CompressionCodec* codecPtr = codec.get();
	aff4::codec::LZ4Compression* derived = dynamic_cast<aff4::codec::LZ4Compression*>(codecPtr);
	CPPUNIT_ASSERT(derived != nullptr);
}

TEST_METHOD(testLZ4Resource) {
	std::string res = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGE_COMPRESSION_LZ4);
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(
			aff4::Lexicon::AFF4_IMAGE_COMPRESSION_LZ4, AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec != nullptr);
	CPPUNIT_ASSERT_EQUAL(res, codec->getResourceID());
	CPPUNIT_ASSERT_EQUAL((uint32_t)AFF4_DEFAULT_CHUNK_SIZE, codec->getChunkSize());

	// Ensure we get the correct derived type.
	aff4::codec::CompressionCodec* codecPtr = codec.get();
	aff4::codec::LZ4Compression* derived = dynamic_cast<aff4::codec::LZ4Compression*>(codecPtr);
	CPPUNIT_ASSERT(derived != nullptr);
}

TEST_METHOD(testDeflate) {
	std::string res = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGE_COMPRESSION_DEFLATE);
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(res, AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec != nullptr);
	CPPUNIT_ASSERT_EQUAL(res, codec->getResourceID());
	CPPUNIT_ASSERT_EQUAL((uint32_t)AFF4_DEFAULT_CHUNK_SIZE, codec->getChunkSize());

	// Ensure we get the correct derived type.
	aff4::codec::CompressionCodec* codecPtr = codec.get();
	aff4::codec::DeflateCompression* derived = dynamic_cast<aff4::codec::DeflateCompression*>(codecPtr);
	CPPUNIT_ASSERT(derived != nullptr);
}

TEST_METHOD(testDeflateResource) {
	std::string res = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGE_COMPRESSION_DEFLATE);
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(
			aff4::Lexicon::AFF4_IMAGE_COMPRESSION_DEFLATE, AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec != nullptr);
	CPPUNIT_ASSERT_EQUAL(res, codec->getResourceID());
	CPPUNIT_ASSERT_EQUAL((uint32_t)AFF4_DEFAULT_CHUNK_SIZE, codec->getChunkSize());

	// Ensure we get the correct derived type.
	aff4::codec::CompressionCodec* codecPtr = codec.get();
	aff4::codec::DeflateCompression* derived = dynamic_cast<aff4::codec::DeflateCompression*>(codecPtr);
	CPPUNIT_ASSERT(derived != nullptr);
}

TEST_METHOD(testNull) {
	std::string res = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGE_COMPRESSION_STORED);
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(res, AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec != nullptr);
	CPPUNIT_ASSERT_EQUAL(res, codec->getResourceID());
	CPPUNIT_ASSERT_EQUAL((uint32_t)AFF4_DEFAULT_CHUNK_SIZE, codec->getChunkSize());

	// Ensure we get the correct derived type.
	aff4::codec::CompressionCodec* codecPtr = codec.get();
	aff4::codec::NullCompression* derived = dynamic_cast<aff4::codec::NullCompression*>(codecPtr);
	CPPUNIT_ASSERT(derived != nullptr);
}

TEST_METHOD(testNullResource) {
	std::string res = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGE_COMPRESSION_STORED);
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(
			aff4::Lexicon::AFF4_IMAGE_COMPRESSION_STORED, AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec != nullptr);
	CPPUNIT_ASSERT_EQUAL(res, codec->getResourceID());
	CPPUNIT_ASSERT_EQUAL((uint32_t)AFF4_DEFAULT_CHUNK_SIZE, codec->getChunkSize());

	// Ensure we get the correct derived type.
	aff4::codec::CompressionCodec* codecPtr = codec.get();
	aff4::codec::NullCompression* derived = dynamic_cast<aff4::codec::NullCompression*>(codecPtr);
	CPPUNIT_ASSERT(derived != nullptr);
}

TEST_METHOD(testBadValue) {
	std::string res = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_CONTAINS);
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(res,
	AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec == nullptr);
}

TEST_METHOD(testBadValueResource) {
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(aff4::Lexicon::AFF4_CONTAINS,
	AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec == nullptr);
}

TEST_METHOD(testNullValue) {
	std::string res = aff4::lexicon::getLexiconString(aff4::Lexicon::UNKNOWN);
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(res, AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec == nullptr);
}

TEST_METHOD(testNullValueResource) {
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec("", AFF4_DEFAULT_CHUNK_SIZE);
	CPPUNIT_ASSERT(codec == nullptr);
}

TEST_METHOD(testNoCompression) {
	std::unique_ptr<char[]> source(new char[blockLength]);
	std::unique_ptr<char[]> destination(new char[blockLength]);

	::memcpy(source.get(), srcText.data(), blockLength);

	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(
			aff4::Lexicon::AFF4_IMAGE_COMPRESSION_STORED, blockLength);

	int res = codec->decompress(source.get(), blockLength, destination.get(), blockLength);
	CPPUNIT_ASSERT_EQUAL(blockLength, res);
	std::string dest(destination.get(), blockLength);
	CPPUNIT_ASSERT_EQUAL(srcText, dest);
}

TEST_METHOD(testSnappyCompression) {
	std::unique_ptr<char[]> source(new char[blockLength]);
	std::unique_ptr<char[]> cSource(new char[blockLength * 2]);
	std::unique_ptr<char[]> destination(new char[blockLength]);

	::memcpy(source.get(), srcText.data(), blockLength);

	// Compress with snappy.
	size_t cres = blockLength * 2;
	snappy::RawCompress((const char*) source.get(), blockLength, cSource.get(), &cres);
	CPPUNIT_ASSERT_EQUAL((uint64_t)83, (uint64_t)cres);

	// Decompress.
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(
			aff4::Lexicon::AFF4_IMAGE_COMPRESSION_SNAPPY, blockLength);

	uint64_t  res = codec->decompress((void*)cSource.get(), 83, destination.get(), blockLength);
	CPPUNIT_ASSERT_EQUAL((uint64_t )blockLength, res);
	std::string dest(destination.get(), blockLength);
	CPPUNIT_ASSERT_EQUAL(srcText, dest);
}

TEST_METHOD(testDeflateCompression) {
	std::unique_ptr<char[]> source(new char[blockLength]);
	std::unique_ptr<char[]> cSource(new char[blockLength * 2]);
	std::unique_ptr<char[]> destination(new char[blockLength]);

	::memcpy(source.get(), srcText.data(), blockLength);

	// Compress with deflate
	unsigned long int cres = blockLength * 2;
	::compress((unsigned char*)cSource.get(), &cres, (const unsigned char*)source.get(), blockLength);

	CPPUNIT_ASSERT_EQUAL((unsigned long int )66, cres);

	// Decompress.
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(
			aff4::Lexicon::AFF4_IMAGE_COMPRESSION_DEFLATE, blockLength);

	uint64_t res = codec->decompress(cSource.get(), 66, destination.get(), blockLength);
	CPPUNIT_ASSERT_EQUAL((uint64_t)blockLength, res);
	std::string dest(destination.get(), blockLength);
	CPPUNIT_ASSERT_EQUAL(srcText, dest);
}

TEST_METHOD(testLZ4Compression) {
// We skip the LZ4 test, as LZ4 is being embedded (and not exported) as part of build process with VC++2015
#ifndef _WIN32
	std::unique_ptr<char[]> source(new char[blockLength]);
	std::unique_ptr<char[]> cSource(new char[blockLength * 2]);
	std::unique_ptr<char[]> destination(new char[blockLength]);

	::memcpy(source.get(), srcText.data(), blockLength);

	// Compress with lz4.
	unsigned long int cres = blockLength * 2;
	cres = ::LZ4_compress_default((const char *)source.get(), (char*)cSource.get(), blockLength, cres);
	CPPUNIT_ASSERT_EQUAL((unsigned long int )84, cres);

	// Decompress.
	std::shared_ptr<aff4::codec::CompressionCodec> codec = aff4::codec::getCodec(
			aff4::Lexicon::AFF4_IMAGE_COMPRESSION_LZ4, blockLength);

	uint64_t res = codec->decompress(cSource.get(), 84, destination.get(), blockLength);
	CPPUNIT_ASSERT_EQUAL((uint64_t)blockLength, res);
	std::string dest(destination.get(), blockLength);
	CPPUNIT_ASSERT_EQUAL(srcText, dest);

#endif
}


#if defined _WIN32 && defined _MSC_VER 

	};

}

#endif
