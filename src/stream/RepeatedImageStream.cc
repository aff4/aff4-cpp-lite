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

#ifdef _WIN32
#define _SCL_SECURE_NO_WARNINGS
#endif

#include "RepeatedImageStream.h"
#include <algorithm> 
#include <inttypes.h>

namespace aff4 {
namespace stream {

/**
 * Predefinition to 1MB.
 */
#define UNITS_M  (1024 * 1024)
/**
 * Address mask for correct calculation of fill pattern alignment.
 */
#define UNITS_MASK (UNITS_M - 1)

RepeatedImageStream::RepeatedImageStream(const std::string& resource, const std::string& value) :
		AFF4Resource(resource), symbol(value) {

	// Add default properties for this stream type.
	addProperty(aff4::Lexicon::AFF4_TYPE, aff4::rdf::RDFValue(aff4::Lexicon::AFF4_IMAGESTREAM_TYPE));
	addProperty(aff4::Lexicon::AFF4_SIZE, aff4::rdf::RDFValue((int64_t) LLONG_MAX));

	// Construct our buffer.
	BUFFER = std::unique_ptr<char[]>(new char[UNITS_M]);
	size_t patternLength = symbol.size();
	char* buffer = BUFFER.get();
	for (size_t i = 0; i < UNITS_M; i += patternLength) {
		size_t limit = std::min((UNITS_M - i), patternLength);
		symbol.copy(buffer, limit, 0);
		buffer += patternLength;
	}

}

RepeatedImageStream::~RepeatedImageStream() {
	close();
}

uint64_t RepeatedImageStream::size() noexcept {
	return ULLONG_MAX;
}

void RepeatedImageStream::close() noexcept {
	// NOP
}

std::string  RepeatedImageStream::getSymbol() noexcept {
	return symbol;
}

int64_t RepeatedImageStream::read(void *buf, uint64_t count, uint64_t offset) noexcept {
	if ((count == 0) || (buf == nullptr)) {
		return 0;
	}
	uint64_t remaining = count;
#if DEBUG
	fprintf(aff4::getDebugOutput(), "%s[%d] : Reading  %" PRIx64 " : %" PRIx64 " \n", __FILE__, __LINE__, offset, count);
#endif

	char* buffer = static_cast<char*>(buf);
	// Specification for repeated pattern ImageStream works on 1MB boundaries.
	uint64_t boffset = offset & UNITS_MASK;
	uint64_t limit = std::min<uint64_t>(remaining, UNITS_M - boffset);
	uint64_t remainder = remaining;
	while (remainder > 0) {
		// fill the buffer with the pattern.
		::memcpy(buffer, (const char*) (BUFFER.get() + boffset), limit);
		boffset = 0;
		remainder -= limit;
		buffer += limit;
		// calculate next read/copy size.
		limit = std::min(remainder, (uint64_t)UNITS_M);
	}
#if DEBUG
	fprintf(aff4::getDebugOutput(), "%s[%d] : Completed Read  %" PRIx64 " : %" PRIx64 " => %" PRIx64 " \n", __FILE__, __LINE__, offset, count, remaining);
#endif
	return remaining;
}

/*
 * AFF4 Resource
 */

std::string RepeatedImageStream::getResourceID() const noexcept {
	return AFF4Resource::getResourceID();
}

aff4::Lexicon RepeatedImageStream::getBaseType() noexcept {
	return aff4::Lexicon::AFF4_IMAGESTREAM_TYPE;
}

std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> RepeatedImageStream::getProperties() noexcept {
	return AFF4Resource::getProperties();
}

std::vector<aff4::rdf::RDFValue> RepeatedImageStream::getProperty(aff4::Lexicon resource) noexcept {
	return AFF4Resource::getProperty(resource);
}

} /* namespace stream */
} /* namespace aff4 */
