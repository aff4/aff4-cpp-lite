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
 * @file LZ4Compression.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 LZ4 Compression Codec
 */

#ifndef SRC_CODEC_LZ4COMPRESSION_H_
#define SRC_CODEC_LZ4COMPRESSION_H_

#include "CompressionCodec.h"

namespace aff4 {
namespace codec {

/**
 * @brief LZ4 Compression Codec
 */
class LZ4Compression: public CompressionCodec {
public:
	/**
	 * Create a new LZ4 compression codec
	 * @param chunkSize The default chunksize.
	 */
	LIBAFF4_API_LOCAL explicit LZ4Compression(uint32_t chunkSize);
	virtual ~LZ4Compression();
	virtual uint64_t decompress(void* source, uint64_t srcSize, void* destination, uint64_t destSize) noexcept;
};

} /* namespace codec */
} /* namespace aff4 */

#endif /* SRC_CODEC_LZ4COMPRESSION_H_ */
