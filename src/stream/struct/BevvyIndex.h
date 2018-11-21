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
 * @file BevvyIndex.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 Bevvy Index Implementation
 *
 * This class provides loading of bevvy indices.
 */
#ifndef SRC_STREAM_STRUCT_BEVVYINDEX_H_
#define SRC_STREAM_STRUCT_BEVVYINDEX_H_

#include "aff4config.h"
#include "aff4.h"

#include "AFF4ZipContainer.h"
#include "ImageStreamPoint.h"

#ifndef AFF4ZipContainer
namespace aff4 {
namespace container {
class AFF4ZipContainer;
}
}
#endif

namespace aff4 {
namespace stream {
namespace structs {

/**
 * @brief Class representing a single bevvy index of a AFF4 Image Stream.
 */
class BevvyIndex {
public:
	/**
	 * Construct a new bevvy index.
	 * @param resource The resource of the image stream we are servicing.
	 * @param bevvyID The bevvy id
	 * @param parent The parent container
	 */
	LIBAFF4_API_LOCAL BevvyIndex(const std::string& resource, uint32_t bevvyID,
			aff4::container::AFF4ZipContainer* parent);
	virtual ~BevvyIndex();

	/**
	 * Get the ID of this bevvy index.
	 * @return The id of this bevvy index.
	 */
	LIBAFF4_API_LOCAL uint32_t getBevvyID() const noexcept;

	/**
	 * Get the image point for this region
	 * @param offset The chunk offset.
	 * @return The ImagePoint.
	 */
	LIBAFF4_API_LOCAL ImageStreamPoint getPoint(uint32_t offset) const noexcept;

	/**
	 * The offset within the parent container, that the data chunk resides.
	 * @return The offset within the container.
	 */
	LIBAFF4_API_LOCAL uint64_t getDataOffset() const noexcept;

private:
	/**
	 * The resource of the image stream we are servicing.
	 */
	std::string resource;
	/**
	 * The bevvy ID
	 */
	const uint32_t bevvyID;
	/**
	 * The offset within the parent, that the data chunk resides.
	 */
	uint64_t dataChunkOffset;
	/**
	 * The parent container instance.
	 */
	aff4::container::AFF4ZipContainer* parent;
	/**
	 * Number of point entries in index.
	 */
	uint64_t size;

	/**
	 * The buffer of points.
	 */
	std::unique_ptr<ImageStreamPoint[]> buffer;
};

} /* namespace structs */
} /* namespace stream */
} /* namespace aff4 */

#endif /* SRC_STREAM_STRUCT_BEVVYINDEX_H_ */
