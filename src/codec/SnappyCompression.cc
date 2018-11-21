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

#include "SnappyCompression.h"

#include <snappy.h>

namespace aff4 {
namespace codec {

SnappyCompression::SnappyCompression(uint32_t chunkSize) :
		CompressionCodec(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGE_COMPRESSION_SNAPPY), chunkSize) {
}

SnappyCompression::~SnappyCompression() {
}

uint64_t SnappyCompression::decompress(void* source, uint64_t srcSize, void* destination, uint64_t destSize) noexcept {
	if (source == nullptr || destination == nullptr) {
		return 0;
	}
	if (snappy::RawUncompress((char*) source, srcSize, (char*) destination)) {
		return destSize;
	} else {
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : Snappy decompression failure\n", __FILE__, __LINE__);
#endif
		return 0;
	}
}

} /* namespace codec */
} /* namespace aff4 */
