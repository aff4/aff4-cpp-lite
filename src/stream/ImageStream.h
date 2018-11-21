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
 * @file ImageStream.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 Zip based ImageStream implementation
 *
 * This class provides implementation of aff4:ImageStream backed by a AFF4 Zip Container.
 */

#ifndef SRC_STREAM_IMAGESTREAM_H_
#define SRC_STREAM_IMAGESTREAM_H_

#include "aff4config.h"
#include "aff4.h"

#include <atomic>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <memory>

#include "Cache.h"

#include "AFF4Resource.h"
#include "AFF4ZipContainer.h"
#include "AFF4Lexicon.h"

#include "CompressionCodec.h"
#include "BevvyIndexLoader.h"


namespace aff4 {
namespace stream {

/**
 * Buffer type used in the chunk cache.
 */
typedef typename std::pair<std::shared_ptr<uint8_t>, uint32_t> cacheBuffer_t;

}
}

#include "ChunkLoader.h"

#ifndef AFF4ZipContainer
namespace aff4 {
namespace container {
class AFF4ZipContainer;
}
}
#endif

#ifndef BevvyIndex
namespace aff4 {
namespace stream {
namespace structs {
class BevvyIndex;
}
}
}
#endif

#ifndef BevvyIndexLoader
namespace aff4 {
namespace stream {
namespace structs {
class BevvyIndexLoader;
}
}
}
#endif

#ifndef ChunkLoader
namespace aff4 {
namespace stream {
namespace structs {
class ChunkLoader;
}
}
}
#endif

namespace aff4 {
namespace stream {

/**
 * @brief Base AFF4 Image Stream.
 *
 * This implementation provides a lightweight LRU cache for data chunks.
 * To set the cache size for the materialised stream see {@link aff4::stream::setImageStreamCacheSize()}.
 */
class ImageStream: public AFF4Resource, public IAFF4Stream {
public:
	/**
	 * Create a new image stream for contents backed by a AFF4 Zip container
	 * @param resource The resource
	 * @param parent The parent container.
	 */
	LIBAFF4_API_LOCAL ImageStream(const std::string& resource, aff4::container::AFF4ZipContainer* parent);
	virtual ~ImageStream();

	/*
	 * From AFF4Resource.
	 */
	std::string getResourceID() const noexcept;
	aff4::Lexicon getBaseType() noexcept;
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> getProperties() noexcept;
	std::vector<aff4::rdf::RDFValue> getProperty(aff4::Lexicon resource) noexcept;

	/*
	 * IAFF4Stream
	 */
	uint64_t size() noexcept;
	void close() noexcept;
	int64_t read(void *buf, uint64_t count, uint64_t offset) noexcept;

private:
	/**
	 * Parent container.
	 */
	aff4::container::AFF4ZipContainer* parent;
	/**
	 * Closed flag
	 */
	std::atomic<bool> closed;
	/**
	 * The length of the stream.
	 */
	uint64_t length;

	/**
	 * The chunkSize
	 */
	uint32_t chunkSize;

	/**
	 * The number of chunks in each segment/bevvy.
	 */
	uint32_t chunksInSegment;
	/**
	 * Compression codec.
	 */
	std::shared_ptr<aff4::codec::CompressionCodec> codec;
	/**
	 * Cache of Bevvy Indexes.
	 */
	std::shared_ptr<aff4::util::cache<uint32_t, std::shared_ptr<aff4::stream::structs::BevvyIndex>>> bevvyIndexCache;

	/**
	 * Bevvy Index loader.
	 */
	std::unique_ptr<aff4::stream::structs::BevvyIndexLoader> bevvyLoader;

	/**
	 * Cache of Bevvy Indexes.
	 */
	std::shared_ptr<aff4::util::cache<uint64_t, cacheBuffer_t>> chunkCache;

	/**
	 * Chunk loader. (handles decompression).
	 */
	std::unique_ptr<aff4::stream::structs::ChunkLoader> chunkLoader;
};

} /* namespace stream */
} /* namespace aff4 */

#endif /* SRC_STREAM_IMAGESTREAM_H_ */
