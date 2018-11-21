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
 * @file SymbolicImageStream.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief A symbolic aff4:stream object implementation.
 *
 * This class defines the default implementation of symbolic streams that are
 */

#ifndef SRC_STREAM_SYMBOLICIMAGESTREAM_H_
#define SRC_STREAM_SYMBOLICIMAGESTREAM_H_

#include "aff4config.h"
#include "aff4.h"

#include <climits>

#include "AFF4Resource.h"

namespace aff4 {
namespace stream {

/**
 * @brief Default symbolic channel implementation
 */
class SymbolicImageStream: public AFF4Resource, public IAFF4Stream {
public:
	/**
	 * Create a new symbolic channel
	 * @param resource The resource ID for this channel
	 * @param value The symbol value to use.
	 */
	LIBAFF4_API explicit SymbolicImageStream(const std::string& resource, uint8_t value);
	/**
	 * Create a new symbolic channel.
	 * <p>
	 * The symbol for this channel will be sourced/guessed from the resource.
	 * @param resource The resource ID for this channel
	 */
	LIBAFF4_API explicit SymbolicImageStream(const std::string& resource);
	virtual ~SymbolicImageStream();

	/**
	 * Get the underlying symbol
	 * @return The underying symbol.
	 */
	LIBAFF4_API uint8_t getSymbol() const noexcept;

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
	 * The underlying symbol.
	 */
	 uint8_t symbol;

	/**
	 * Initialise object properties.
	 */
	LIBAFF4_API_LOCAL void initProperties() noexcept;
};

} /* namespace stream */
} /* namespace aff4 */

#endif /* SRC_STREAM_SYMBOLICIMAGESTREAM_H_ */
