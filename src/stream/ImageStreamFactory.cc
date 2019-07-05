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

#include "ImageStreamFactory.h"
namespace aff4 {
namespace stream {

std::shared_ptr<IAFF4Stream> createZeroStream() {
#if DEBUG
	fprintf(aff4::getDebugOutput(), "%s[%d] : Create aff4:Zero stream \n", __FILE__, __LINE__);
#endif
	return std::make_shared<SymbolicImageStream>(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_ZERO), (uint8_t) 0);
}

std::shared_ptr<IAFF4Stream> createUnknownStream() {
#if DEBUG
	fprintf(aff4::getDebugOutput(), "%s[%d] : Create aff4:Unknown stream \n", __FILE__, __LINE__);
#endif
	return std::make_shared<RepeatedImageStream>(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_UNKNOWN), "UNKNOWN");
}

std::shared_ptr<IAFF4Stream> createUnknownStream(const std::string& resource) {
#if DEBUG
	fprintf(aff4::getDebugOutput(), "%s[%d] : Create aff4:Unknown stream with Resource %s \n", __FILE__, __LINE__, resource.c_str());
#endif
	return std::make_shared<RepeatedImageStream>(resource, "UNKNOWN");
}

std::shared_ptr<IAFF4Stream> createUnreadableStream() {
#if DEBUG
	fprintf(aff4::getDebugOutput(), "%s[%d] : Create aff4:Unreadable stream \n", __FILE__, __LINE__);
#endif
	return std::make_shared<RepeatedImageStream>(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_UNREADABLE), "UNREADABLEDATA");
}

std::shared_ptr<IAFF4Stream> createSymbolicStream(const std::string& resource) {
#if DEBUG
	fprintf(aff4::getDebugOutput(), "%s[%d] : Create aff4:Symbolic stream %s \n", __FILE__, __LINE__, resource.c_str());
#endif
	return std::make_shared<SymbolicImageStream>(resource);
}

std::shared_ptr<IAFF4Stream> createSymbolicStream(uint8_t symbol) {
#if DEBUG
	fprintf(aff4::getDebugOutput(), "%s[%d] : Create aff4:Symbolic stream %02X \n", __FILE__, __LINE__, symbol);
#endif
	std::string resource = aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_SYMBOLIC_PREFIX);
	char buf[4];
	std::snprintf(buf, 3, "%02X", symbol);
	resource += std::string(buf, 2);
	return std::make_shared<SymbolicImageStream>(resource, symbol);
}

} /* namespace stream */
} /* namespace aff4 */
