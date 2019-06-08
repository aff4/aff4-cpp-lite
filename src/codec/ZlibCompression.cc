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

#include "ZlibCompression.h"
#include "zlib.h"

namespace aff4 {
namespace codec {

ZlibCompression::ZlibCompression(uint32_t chunkSize) :
		CompressionCodec(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGE_COMPRESSION_ZLIB), chunkSize) {
}

ZlibCompression::~ZlibCompression() {
	// NOP
}

uint64_t ZlibCompression::decompress(void* source, uint64_t srcSize, void* destination, uint64_t destSize) noexcept {
	if (source == nullptr || destination == nullptr) {
		return 0;
	}
	unsigned long int destConsumed = (unsigned long int)destSize;
	::uncompress((unsigned char*)destination, &destConsumed, (unsigned char*)source, (unsigned long int)srcSize);
	return (uint64_t)destConsumed;
}

} /* namespace codec */
} /* namespace aff4 */
