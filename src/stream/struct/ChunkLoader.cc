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

#include "ChunkLoader.h"
#include <inttypes.h>

namespace aff4 {
namespace stream {
namespace structs {

ChunkLoader::ChunkLoader(const std::string& resource, aff4::container::AFF4ZipContainer* parent,
		std::shared_ptr<aff4::util::cache<uint32_t, std::shared_ptr<aff4::stream::structs::BevvyIndex>>>& bevvyCache,
		uint32_t chunkSize, uint32_t chunksInSegment, std::shared_ptr<aff4::codec::CompressionCodec>& codec) :
		resource(resource), parent(parent), bevvyCache(bevvyCache), chunkSize(chunkSize), chunksInSegment(
				chunksInSegment), codec(codec) {
}

ChunkLoader::~ChunkLoader() {
	// NOP
	parent = nullptr;
}

cacheBuffer_t ChunkLoader::load(uint64_t offset) {

#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Loading Buffer: %" PRIu64 " \n", __FILE__, __LINE__, offset);
#endif

	// Determine the bevvy ID.
	uint64_t bevvyID = (offset / chunkSize) / chunksInSegment;
	std::shared_ptr<BevvyIndex> index = bevvyCache->get((uint32_t) bevvyID);
	if (index == nullptr) {
		// failed to load.
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : Failed to acquire Bevvy Index %" PRIu64 " for Buffer: %" PRIu64 " \n", __FILE__,
		__LINE__, bevvyID, offset);
#endif
		return std::make_pair(nullptr, 0);
	}

	// Determine the offset into the bevvy index our chunk is.
	uint64_t chunkID = (offset / chunkSize) % chunksInSegment;
	ImageStreamPoint point = index->getPoint((uint32_t) chunkID);
	if (point.length == 0) {
		// point has no length
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : Failed to read Bevvy Index Point (bevvy: %" PRIu64 ") for Buffer: %" PRIu64 " \n",
		__FILE__, __LINE__, bevvyID, offset);
#endif
		return std::make_pair(nullptr, 0);
	}

	uint64_t chunkOffset = index->getDataOffset() + point.offset;
	uint64_t chunkLength = point.length;

	/*
	 * Chunk Offset and Chunk Length are for offsets into the direct ZIP level container. (we really
	 * should get a IAFF4Stream for the zip segment, but lets shortcut and just read directly from the
	 * underlying container).
	 */

	// Create a buffer to read in our compressed data block.
	std::shared_ptr<uint8_t> buffer(new uint8_t[chunkLength], std::default_delete<uint8_t[]>());
	uint64_t toRead = chunkLength;
	uint8_t* buf = buffer.get();
	while (toRead > 0) {
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : Reading Chunk [%" PRIu64 ":%" PRIu64 "] for Buffer: %" PRIu64 " \n",
		__FILE__, __LINE__, chunkOffset, toRead, offset);
#endif
		// In all typical circumstances this should be a single read, but be careful otherwise.
		uint64_t res = parent->fileRead(buf, toRead, chunkOffset);
		if (res == 0) {
			break;
		}
		toRead -= res;
		chunkOffset += res;
		buf += res;
	}
	if (chunkLength != chunkSize) {
		// decompress
		std::shared_ptr<uint8_t> dest(new uint8_t[chunkSize], std::default_delete<uint8_t[]>());
		codec->decompress(buffer.get(), chunkLength, dest.get(), chunkSize);
		return std::make_pair(dest, chunkSize);
	}
	// No decompression needed, just return.
	return std::make_pair(buffer, chunkSize);
}

} /* namespace structs */
} /* namespace stream */
} /* namespace aff4 */
