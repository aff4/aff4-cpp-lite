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
 * @file ImageStreamFactory.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief Base methods for create aff4:ImageStream for symbolic stream types.
 *
 * This file defines helper methods for creation of symbolic streams.
 */

#ifndef SRC_STREAM_IMAGESTREAMFACTORY_H_
#define SRC_STREAM_IMAGESTREAMFACTORY_H_

#include "aff4config.h"
#include "aff4.h"

#include <cstdio>

#include "SymbolicImageStream.h"
#include "RepeatedImageStream.h"

namespace aff4 {
/**
 * @brief Base aff4:stream (aff4::IAFF4Stream) implementations.
 */
namespace stream {

	/**
	 * Create an aff4:Zero Image Stream
	 *
	 * @return the constructed image stream.
	 */
	LIBAFF4_API std::shared_ptr<IAFF4Stream> createZeroStream();
	/**
	 * Create an aff4:UnknownData Image Stream
	 *
	 * @return the constructed image stream.
	 */
	LIBAFF4_API std::shared_ptr<IAFF4Stream> createUnknownStream();
	/**
	 * Create an aff4:UnknownData Image Stream
	 *
	 * @param resource The name resource string to use for this stream.
	 * @return the constructed image stream.
	 */
	LIBAFF4_API std::shared_ptr<IAFF4Stream> createUnknownStream(const std::string& resource);

	/**
	 * Create an aff4:UnreadableData Image Stream
	 *
	 * @return the constructed image stream.
	 */
	LIBAFF4_API std::shared_ptr<IAFF4Stream> createUnreadableStream();

	/**
	 * Create an aff4:SymbolicStreamXX Image Stream
	 *
	 * @param symbol The symbol for the image stream
	 * @return the constructed image stream.
	 */
	LIBAFF4_API std::shared_ptr<IAFF4Stream> createSymbolicStream(uint8_t symbol);

	/**
	 * Create an aff4:SymbolicStreamXX Image Stream
	 *
	 * @param resource The resource for the image stream
	 * @return the constructed image stream.
	 */
	LIBAFF4_API std::shared_ptr<IAFF4Stream> createSymbolicStream(const std::string& resource);

} /* namespace stream */
} /* namespace aff4 */

#endif
