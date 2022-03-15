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

#include "CompressionCodec.h"
#include "NullCompression.h"
#include "SnappyCompression.h"
#include "DeflateCompression.h"
#include "ZlibCompression.h"
#include "LZ4Compression.h"

namespace aff4 {
namespace codec {

std::shared_ptr<CompressionCodec> getCodec(const std::string& resource, uint32_t chunkSize) {
	aff4::Lexicon codec = aff4::lexicon::getLexicon(resource);
	return getCodec(codec, chunkSize);
}

std::shared_ptr<CompressionCodec> getCodec(aff4::Lexicon resource, uint32_t chunkSize) {
	switch (resource) {
	case Lexicon::AFF4_IMAGE_COMPRESSION_SNAPPY:
	case Lexicon::AFF4_IMAGE_COMPRESSION_SNAPPY2:
#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : Create Snappy Decompressor \n", __FILE__, __LINE__);
#endif
		return std::make_shared<SnappyCompression>(chunkSize);
	case Lexicon::AFF4_IMAGE_COMPRESSION_LZ4:
#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : Create LZ4 Decompressor \n", __FILE__, __LINE__);
#endif
		return std::make_shared<LZ4Compression>(chunkSize);
	case Lexicon::AFF4_IMAGE_COMPRESSION_DEFLATE:
#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : Create Deflate Decompressor \n", __FILE__, __LINE__);
#endif
		return std::make_shared<DeflateCompression>(chunkSize);
	case Lexicon::AFF4_IMAGE_COMPRESSION_STORED:
#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : Create Stored Decompressor \n", __FILE__, __LINE__);
#endif
		return std::make_shared<NullCompression>(chunkSize);
	case Lexicon::AFF4_IMAGE_COMPRESSION_ZLIB:
#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : Create ZLib Decompressor \n", __FILE__, __LINE__);
#endif
		return std::make_shared<ZlibCompression>(chunkSize);
	default:
		break;
	}
#if DEBUG_VERBOSE
	fprintf( aff4::getDebugOutput(), "%s[%d] : Unknown Compression Codec : %s\n", __FILE__, __LINE__,
			aff4::lexicon::getLexiconString(resource).c_str());
#endif
	return nullptr;
}

CompressionCodec::CompressionCodec(const std::string& resource, uint32_t chunkSize) :
		resource(resource), chunkSize(chunkSize) {
}

CompressionCodec::~CompressionCodec() {
	// NOP
}

std::string CompressionCodec::getResourceID() const noexcept {
	return resource;
}

uint32_t CompressionCodec::getChunkSize() const noexcept {
	return chunkSize;
}

} /* namespace codec */
} /* namespace aff4 */
