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

/**
 * @file ChunkLoader.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 data chunk loader
 *
 * This class provides function methods for the cache loader.
 */
#ifndef SRC_STREAM_STRUCT_CHUNKLOADER_H_
#define SRC_STREAM_STRUCT_CHUNKLOADER_H_

#include "aff4config.h"
#include "aff4.h"

#include "AFF4ZipContainer.h"
#include "BevvyIndex.h"
#include "CompressionCodec.h"

namespace aff4 {
namespace stream {
namespace structs {

/**
 * @brief Data Chunk loader function (cache).
 */
class ChunkLoader {
public:
	/**
	 * Create a new Chunk Load tied to the given
	 * @param resource The named resource.
	 * @param parent The parent container
	 * @param bevvyCache The bevvy cache to get the bevvy index from
	 * @param chunkSize The chunk size of the image stream.
	 * @param chunksInSegment The number of chunks per segment
	 * @param codec The compression codec.
	 */
	LIBAFF4_API_LOCAL ChunkLoader(const std::string& resource, aff4::container::AFF4ZipContainer* parent,
			std::shared_ptr<aff4::util::cache<uint32_t, std::shared_ptr<aff4::stream::structs::BevvyIndex>>>& bevvyCache,
			uint32_t chunkSize, uint32_t chunksInSegment, std::shared_ptr<aff4::codec::CompressionCodec>& codec);

	virtual ~ChunkLoader();

	/**
	 * Load the given data chunk instance
	 * @param offset The offset into the Image Stream to acquire.
	 * @return A cache buffer entry.
	 */
	LIBAFF4_API cacheBuffer_t load(uint64_t offset);

private:
	/**
	 * The name resource of this stream
	 */
	const std::string resource;
	/**
	 * The parent container
	 */
	aff4::container::AFF4ZipContainer* parent;
	/**
	 * The bevvy cache
	 */
	std::shared_ptr<aff4::util::cache<uint32_t, std::shared_ptr<aff4::stream::structs::BevvyIndex>>> bevvyCache;
	/**
	 * The data chunk size
	 */
	uint32_t chunkSize;
	/**
	 * The number of chunks in each segment
	 */
	uint32_t chunksInSegment;
	/**
	 * The compression codec in use.
	 */
	std::shared_ptr<aff4::codec::CompressionCodec> codec;
};

} /* namespace structs */
} /* namespace stream */
} /* namespace aff4 */

#endif /* SRC_STREAM_STRUCT_CHUNKLOADER_H_ */
