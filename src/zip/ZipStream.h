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
 * @file ZipStream.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 IAFF4Stream object backed by a ZipSegment
 *
 * This class provides a ZipSegment as a generic IAFF4Stream object.
 */

#ifndef SRC_ZIP_ZIPSTREAM_H_
#define SRC_ZIP_ZIPSTREAM_H_

#include "aff4config.h"
#include "aff4.h"

#include <atomic>

#include "AFF4Resource.h"
#include "Zip.h"

namespace aff4 {
namespace stream {

/**
 * @brief AFF4 IAFF4Stream object backed by a ZipSegment
 *
 * This class provides a ZipSegment as a generic IAFF4Stream object.
 */
class ZipSegmentStream: public AFF4Resource, public IAFF4Stream {
public:
	/**
	 *Create a new ZipSegment Stream
	 * @param resource The resource ID of this stream (typically the segment name).
	 * @param entry The Zip Entry
	 * @param container The underlying zip container.
	 */
	ZipSegmentStream(const std::string& resource, std::shared_ptr<aff4::zip::ZipEntry> entry, aff4::zip::Zip* container);
	~ZipSegmentStream();

	/*
	 * IAFF4Stream
	 */
	uint64_t size() noexcept;
	void close() noexcept;
	int64_t read(void *buf, uint64_t count, uint64_t offset) noexcept;

	/*
	 * From AFF4Resource.
	 */
	std::string getResourceID() const noexcept;
	aff4::Lexicon getBaseType() noexcept;
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> getProperties() noexcept;
	std::vector<aff4::rdf::RDFValue> getProperty(aff4::Lexicon resource) noexcept;

private:
	/**
	 * Closed flag
	 */
	std::atomic<bool> closed;
	/**
	 * Parent Zip Entry details
	 */
	std::shared_ptr<aff4::zip::ZipEntry> entry;
	/**
	 * Zip file file.
	 */
	aff4::zip::Zip* container;

	/**
	 * Read/materialise from a compressed stream.
	 * @param buf A pointer to the buffer to read to.
	 * @param count The number of bytes to read
	 * @param offset The offset from the start of the stream.
	 * @return The number of bytes read. (0 indicates nothing read, or -1 indicates error).
	 */
	int64_t readCompressed(void *buf, uint64_t count, uint64_t offset) noexcept;

};

} /* namespace zip */
} /* namespace aff4 */

#endif /* SRC_ZIP_ZIPSTREAM_H_ */
