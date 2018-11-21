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
 * @file CompressionCodec.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 Compression Codec interface and Factory Methods
 *
 * This class provides the interface for all compression codecs, and factory methods for generating instances.
 */

#ifndef SRC_CODEC_COMPRESSIONCODEC_H_
#define SRC_CODEC_COMPRESSIONCODEC_H_

#include "aff4config.h"
#include "aff4.h"


namespace aff4 {
/**
 * @brief Base compression codec implementations.
 */
namespace codec {

/**
 * @brief Default compression codec interface.
 */
class CompressionCodec {
protected:
	/**
	 * Create a new compression codec.
	 * @param resource The resource name of the compression codec.
	 * @param chunkSize The default chunksize expected.
	 */
	LIBAFF4_API CompressionCodec(const std::string& resource, uint32_t chunkSize);
	virtual ~CompressionCodec();

public:
	/**
	 * Decompress the given buffer.
	 * <p>
	 * If source and destination are overlapping, the behavior is undefined. (any may lead to very, very bad things).
	 * @param source The source buffer
	 * @param srcSize The size of the source buffer
	 * @param destination The destination buffer
	 * @param destSize The size of the destination buffer. (must be at least chunkSize).
	 * @return The number of number of bytes consumed in the destination buffer.
	 */
	LIBAFF4_API virtual uint64_t decompress(void* source, uint64_t srcSize, void* destination,
			uint64_t destSize) = 0;

	/**
	 * Get the resource ID of this codec
	 * @return The resource ID of this codec
	 */
	LIBAFF4_API std::string getResourceID() const noexcept;

	/**
	 * Get the chunkSize expected for this codec.
	 * @return The chunkSize.
	 */
	LIBAFF4_API uint32_t getChunkSize() const noexcept;

protected:
	/**
	 * The named resource of this compression codec.
	 */
	const std::string resource;
	/**
	 * The default chunk size.
	 */
	const uint32_t chunkSize;
};

/*
 * Factory methods.
 */

/**
 * Create Compression Codec for the given type
 * @param resource The resource of the compression codec.
 * @param chunkSize The required chunksize.
 * @return A Compression Codec.
 */
LIBAFF4_API std::shared_ptr<CompressionCodec> getCodec(const std::string& resource, uint32_t chunkSize);
/**
 * Create Compression Codec for the given type
 * @param resource The resource of the compression codec.
 * @param chunkSize The required chunksize.
 * @return A Compression Codec.
 */
LIBAFF4_API std::shared_ptr<CompressionCodec> getCodec(aff4::Lexicon resource, uint32_t chunkSize);

} /* namespace codec */
} /* namespace aff4 */

#endif /* SRC_CODEC_COMPRESSIONCODEC_H_ */
