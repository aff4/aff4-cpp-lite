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

#include "TestUtilities.h"

#if defined _WIN32 && defined _MSC_VER  

 /*
 * MS CPPUNIT
 */
#ifndef CPPUNIT_ASSERT
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define CPPUNIT_ASSERT Assert::IsTrue

#endif
#endif

namespace aff4 {
namespace test {

void printResourceProperties(std::shared_ptr<aff4::IAFF4Resource> image) {
	printf("Object: %s\n", image->getResourceID().c_str());
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> properties = image->getProperties();
	for (auto property = properties.begin(); property != properties.end(); property++) {
		aff4::Lexicon p = property->first;
		std::vector<aff4::rdf::RDFValue> values = property->second;
		printf(" Property: %s\n", aff4::lexicon::getLexiconString(p).c_str());
		for (auto valIt = values.begin(); valIt != values.end(); valIt++) {
			aff4::rdf::RDFValue v = *valIt;
			printf("   %s\n", v.toString().c_str());
		}
	}
	printf("\n");
}

void testPropertyResource(std::shared_ptr<aff4::IAFF4Resource> image, aff4::Lexicon property, std::string expected) {
	std::vector<aff4::rdf::RDFValue> values = image->getProperty(property);
	CPPUNIT_ASSERT(values.size() >= 1);
	for (auto it = values.begin(); it != values.end(); it++) {
		aff4::rdf::RDFValue v = *it;
		if (v.getXSDType() == aff4::rdf::XSDType::Resource) {
			if (v.getValue() == expected) {
				CPPUNIT_ASSERT(true);
				return;
			}
		}
	}
	CPPUNIT_ASSERT(false);
}

void testProperty(std::shared_ptr<aff4::IAFF4Resource> image, aff4::Lexicon property, std::string expected) {
	std::vector<aff4::rdf::RDFValue> values = image->getProperty(property);
	CPPUNIT_ASSERT(values.size() >= 1);
	for (auto it = values.begin(); it != values.end(); it++) {
		aff4::rdf::RDFValue v = *it;
		if (v.getXSDType() == aff4::rdf::XSDType::String) {
			if (v.getValue() == expected) {
				CPPUNIT_ASSERT(true);
				return;
			}
		}
	}
	CPPUNIT_ASSERT(false);
}

void testProperty(std::shared_ptr<aff4::IAFF4Resource> image, aff4::Lexicon property, aff4::Lexicon expected) {
	std::vector<aff4::rdf::RDFValue> values = image->getProperty(property);
	CPPUNIT_ASSERT(values.size() >= 1);
	for (auto it = values.begin(); it != values.end(); it++) {
		aff4::rdf::RDFValue v = *it;
		if (v.getXSDType() == aff4::rdf::XSDType::Resource) {
			if (v.getType() == expected) {
				CPPUNIT_ASSERT(true);
				return;
			}
		}
	}
	CPPUNIT_ASSERT(false);
}

void testProperty(std::shared_ptr<aff4::IAFF4Resource> image, aff4::Lexicon property, int32_t expected) {
	std::vector<aff4::rdf::RDFValue> values = image->getProperty(property);
	CPPUNIT_ASSERT(values.size() >= 1);
	for (auto it = values.begin(); it != values.end(); it++) {
		aff4::rdf::RDFValue v = *it;
		if (v.getXSDType() == aff4::rdf::XSDType::Int) {
			if (v.getInteger() == expected) {
				CPPUNIT_ASSERT(true);
				return;
			}
		}
	}
	CPPUNIT_ASSERT(false);
}

void testProperty(std::shared_ptr<aff4::IAFF4Resource> image, aff4::Lexicon property, int64_t expected) {
	std::vector<aff4::rdf::RDFValue> values = image->getProperty(property);
	CPPUNIT_ASSERT(values.size() >= 1);
	for (auto it = values.begin(); it != values.end(); it++) {
		aff4::rdf::RDFValue v = *it;
		if (v.getXSDType() == aff4::rdf::XSDType::Long) {
			if (v.getLong() == expected) {
				CPPUNIT_ASSERT(true);
				return;
			}
		}
	}
	CPPUNIT_ASSERT(false);
}

std::string sha1sum(std::shared_ptr<IAFF4Stream> stream, uint64_t readSize) {
	unsigned char hash[SHA_DIGEST_LENGTH];
	char hashStr[(SHA_DIGEST_LENGTH * 2) + 2];

	if (stream.get() == nullptr) {
		return "Failed: no stream?";
	}
	SHA_CTX ctx;
	SHA1_Init(&ctx);

	uint64_t toRead = stream->size();
	uint64_t offset = 0;
	uint64_t bufferSize = readSize;
	// Read 128K chunks.
	std::unique_ptr<uint8_t[]> buffer(new uint8_t[bufferSize]);
	if (buffer.get() == nullptr) {
		return "Failed buffer malloc()";
	}
	while (toRead > 0) {
		uint64_t sz = std::min<uint64_t>(bufferSize, toRead);
		int64_t res = stream->read(buffer.get(), sz, offset);

		if (res > 0) {
			SHA1_Update(&ctx, buffer.get(), res);
			toRead -= res;
		} else {
			// read failed?
			toRead = 0;
		}
		offset += res;
	}

	SHA1_Final(hash, &ctx);

	// Convert to string.
	for (int j = 0; j < SHA_DIGEST_LENGTH; j++) {
		::sprintf(&hashStr[2 * j], "%02x", hash[j]);
	}
	std::string result((char*)(&hashStr), (SHA_DIGEST_LENGTH * 2));
	return result;
}

std::string sha1sum(int handle, uint64_t toRead, uint64_t readSize) {

	unsigned char hash[SHA_DIGEST_LENGTH];
	char hashStr[(SHA_DIGEST_LENGTH * 2) + 2];

	SHA_CTX ctx;
	SHA1_Init(&ctx);
	uint64_t offset = 0;
	uint64_t bufferSize = readSize;
	// Read 128K chunks.
	std::unique_ptr<uint8_t[]> buffer(new uint8_t[bufferSize]);
	if (buffer.get() == nullptr) {
		return "Failed buffer malloc()";
	}
	while (toRead > 0) {
		uint64_t sz = std::min<uint64_t>(bufferSize, toRead);
		int64_t res = AFF4_read(handle, offset, buffer.get(), sz);

		if (res > 0) {
			SHA1_Update(&ctx, buffer.get(), res);
			toRead -= res;
		} else {
			// read failed?
			toRead = 0;
		}
		offset += res;
	}

	SHA1_Final(hash, &ctx);

	// Convert to string.
	for (int j = 0; j < SHA_DIGEST_LENGTH; j++) {
		::sprintf(&hashStr[2 * j], "%02x", hash[j]);
	}
	std::string result((char*) &hashStr, (SHA_DIGEST_LENGTH * 2));
	return result;
}

} /* namespace test */
} /* namespace aff4 */
