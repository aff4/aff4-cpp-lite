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
 * @file RepeatedImageStream.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief A symbolic aff4:stream object implementation for streams that contain repeated data patterns.
 *
 * This class defines the default implementation of symbolic streams that contain repeated data patterns.
 */

#ifndef SRC_STREAM_REPEATEDIMAGESTREAM_H_
#define SRC_STREAM_REPEATEDIMAGESTREAM_H_

#include "aff4config.h"
#include "aff4.h"

#include <climits>

#include "AFF4Resource.h"

namespace aff4 {
namespace stream {

/**
 * @brief Default repeated pattern symbolic channel implementation
 */
class RepeatedImageStream: public AFF4Resource, public IAFF4Stream {
public:
	/**
	 * Create a new symbolic channel for a repeated data pattern.
	 * @param resource The resource ID for this channel
	 * @param value The symbol value to use.
	 */
	LIBAFF4_API explicit RepeatedImageStream(const std::string& resource, const std::string& value);
	virtual ~RepeatedImageStream();

	/**
	 * Get the underlying symbol
	 * @return The underying symbol.
	 */
	LIBAFF4_API std::string  getSymbol() noexcept;

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
	 * The pattern
	 */
	std::string symbol;
	/**
	 * Materialised buffer of PATTERN.
	 */
	std::unique_ptr<char[]> BUFFER;
};

} /* namespace stream */
} /* namespace aff4 */

#endif /* SRC_STREAM_REPEATEDIMAGESTREAM_H_ */
