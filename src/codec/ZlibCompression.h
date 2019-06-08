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
 * @file ZlibCompression.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 Zlib Compression Codec (defunct)
 */

#ifndef SRC_CODEC_ZLIBCOMPRESSION_H_
#define SRC_CODEC_ZLIBCOMPRESSION_H_

#include "CompressionCodec.h"

namespace aff4 {
namespace codec {

/**
 * @brief Zlib Compression Codec (defunct). Only provided for legacy containers.
 */
class ZlibCompression: public CompressionCodec {
public:
	/**
	 * Create a new Zlib compression codec
	 * @param chunkSize The default chunksize.
	 */
	LIBAFF4_API explicit ZlibCompression(uint32_t chunkSize);
	virtual ~ZlibCompression();
	virtual uint64_t decompress(void* source, uint64_t srcSize, void* destination, uint64_t destSize) noexcept;
};

} /* namespace codec */
} /* namespace aff4 */

#endif /* SRC_CODEC_ZLIBCOMPRESSION_H_ */
