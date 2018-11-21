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

#include "utils.h"
#include <algorithm>

const static std::string aff4Prefix(AFF4_BASE_URI);

/**
 * Does the string has the given starting string
 * @param value The filename
 * @param prefix the prefix.
 * @return TRUE if the string has the given extension.
 */
bool hasPrefix(std::string const &value, std::string const &prefix) {
	if (value.length() >= prefix.length()) {
		return (!value.compare(0, prefix.length(), prefix));
	} else {
		return false;
	}
}

/**
 * Set short aff4 prefix.
 * @param value
 * @return
 */
std::string setAFF4Prefix(std::string value) {
	if (hasPrefix(value, aff4Prefix)) {
		value = "aff4:" + value.substr(aff4Prefix.length());
	}
	return value;
}

/**
 * Print all values.
 * @param values The values to print.
 */
void printElements(const std::vector<aff4::rdf::RDFValue>& values) {

	bool first = true;
	for (aff4::rdf::RDFValue value : values) {
		if (!first) {
			std::cout << ", ";
		} else {
			first = false;
		}
		std::cout << setAFF4Prefix(value.toString());
	}
	std::cout << std::endl;
}

/**
 * Print all elements of this resource
 * @param resource The resource
 * @param spacer The spacer for displaying values...
 */
void printProperties(std::shared_ptr<aff4::IAFF4Resource> resource, const std::string& spacer) {
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> properties = resource->getProperties();

	/*
	 * Print the object types.
	 */
	for (auto it = properties.begin(); it != properties.end(); it++) {
		aff4::Lexicon t = it->first;
		if (t == aff4::Lexicon::AFF4_TYPE) {
			std::cout << spacer << "AFF4 Types: ";
			printElements(it->second);
		}
	}
	/*
	 * Print all other values.
	 */

	for (auto it = properties.begin(); it != properties.end(); it++) {
		aff4::Lexicon t = it->first;

		if (t != aff4::Lexicon::AFF4_TYPE) {
			std::cout << spacer << setAFF4Prefix(aff4::lexicon::getLexiconString(t)) << ": ";
			printElements(it->second);
		}
	}
}

std::string sha1sum(std::shared_ptr<aff4::IAFF4Stream> stream, uint64_t readSize) {
	if (stream.get() == nullptr) {
		return "Failed: no stream?";
	}

	unsigned char hash[SHA_DIGEST_LENGTH];
	char hashStr[(SHA_DIGEST_LENGTH * 2) + 2];

	SHA_CTX ctx;
	SHA1_Init(&ctx);

	uint64_t count = 0;
	uint64_t countMarker = 128 * 1024 * 1024;

	uint64_t toRead = stream->size();
	uint64_t offset = 0;
	uint64_t bufferSize = readSize;
	// Read in buffer fulls
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

		count += res;
		if(count > countMarker){
			std::cout << ".";
			std::cout.flush();
			count = 0;
		}
	}
	std::cout << std::endl;
	SHA1_Final(hash, &ctx);

	// Convert to string.
	for (int j = 0; j < SHA_DIGEST_LENGTH; j++) {
		::sprintf(&hashStr[2 * j], "%02x", hash[j]);
	}
	std::string result((char*) &hashStr, (SHA_DIGEST_LENGTH * 2));
	return result;
}
