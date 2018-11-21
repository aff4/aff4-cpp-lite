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

#include "SymbolicImageStream.h"
#include "StringUtil.h"

namespace aff4 {
namespace stream {

SymbolicImageStream::SymbolicImageStream(const std::string& resource, uint8_t value) :
		AFF4Resource(resource), symbol(value) {

	// Add default properties for this stream type.
	initProperties();
}

SymbolicImageStream::SymbolicImageStream(const std::string& resource) :
		AFF4Resource(resource), symbol(0) {

	std::string prefix = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_SYMBOLIC_PREFIX);
	if (aff4::util::hasPrefix(resource, prefix)) {
		std::string value = resource.substr(prefix.size());
		try {
			symbol = std::stoi(value, nullptr, 16);
		} catch (...) {
			// ignore.
#if DEBUG
			fprintf( aff4::getDebugOutput(), "%s[%d] : Unable to determine symbol for symbolic stream : %s\n", __FILE__, __LINE__,
					resource.c_str());
#endif
		}
	}
	// Add default properties for this stream type.
	initProperties();
}

void SymbolicImageStream::initProperties() noexcept {
	// Add default properties for this stream type.
	addProperty(aff4::Lexicon::AFF4_TYPE, aff4::rdf::RDFValue(aff4::Lexicon::AFF4_IMAGESTREAM_TYPE));
	addProperty(aff4::Lexicon::AFF4_SIZE, aff4::rdf::RDFValue((int64_t) LLONG_MAX));
}

SymbolicImageStream::~SymbolicImageStream() {
	close();
}

uint8_t SymbolicImageStream::getSymbol() const noexcept {
	return symbol;
}

uint64_t SymbolicImageStream::size() noexcept {
	return ULLONG_MAX;
}

void SymbolicImageStream::close() noexcept {
	// NOP
}

int64_t SymbolicImageStream::read(void *buf, uint64_t count, uint64_t offset) noexcept {
	(void) offset;
	if ((count == 0) || (buf == nullptr)) {
		return 0;
	}
	::memset(buf, symbol, count);
	return count;
}

/*
 * AFF4 Resource
 */

std::string SymbolicImageStream::getResourceID() const noexcept {
	return AFF4Resource::getResourceID();
}

aff4::Lexicon SymbolicImageStream::getBaseType() noexcept {
	return aff4::Lexicon::AFF4_IMAGESTREAM_TYPE;
}

std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> SymbolicImageStream::getProperties() noexcept {
	return AFF4Resource::getProperties();
}

std::vector<aff4::rdf::RDFValue> SymbolicImageStream::getProperty(aff4::Lexicon resource) noexcept {
	return AFF4Resource::getProperty(resource);
}

} /* namespace stream */
} /* namespace aff4 */
