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

#include "BevvyIndex.h"
#include <inttypes.h>
#include "PortableEndian.h"

namespace aff4 {
namespace stream {
namespace structs {

BevvyIndex::BevvyIndex(const std::string& resource, uint32_t bevvyID, aff4::container::AFF4ZipContainer* parent) :
		resource(resource), bevvyID(bevvyID), dataChunkOffset(0), parent(parent), size(0) {

	// Get the ZipEntry to get the start of the actual data...
	std::string segmentName = resource + "/";
	char buf[9];
	std::snprintf(buf, 9, "%08" PRIu32, bevvyID);
	segmentName += std::string(buf, 8);

	std::shared_ptr<aff4::zip::ZipEntry> zipEntry = parent->getSegmentEntry(segmentName);
	if (zipEntry == nullptr) {
		// WTF?
		buffer = nullptr;
		return;
	}
	dataChunkOffset = zipEntry->getOffset();

	// Load the contents of the bevvy Index.
	segmentName = segmentName + ".index";
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Loading Bevvy Index : %s \n", __FILE__, __LINE__, segmentName.c_str());
#endif
	std::shared_ptr<aff4::IAFF4Stream> stream = parent->getSegment(segmentName);
	if (stream == nullptr) {
#if DEBUG
		fprintf(aff4::getDebugOutput(), "%s[%d] : Loading Bevvy Index Failed - missing segment? \n", __FILE__, __LINE__);
#endif
		buffer = nullptr;
		return;
	}
	uint64_t streamSize = stream->size();
	if (streamSize > 0) {
		size = streamSize / sizeof(ImageStreamPoint);
		buffer = std::unique_ptr<ImageStreamPoint[]>(new ImageStreamPoint[size]);
		stream->read(buffer.get(), streamSize, 0);
	}
#if DEBUG
	fprintf(aff4::getDebugOutput(), "%s[%d] : Loading Bevvy Index Size %" PRIu64 "? \n", __FILE__, __LINE__, streamSize);
#endif
	stream->close();
#if __BYTE_ORDER == __BIG_ENDIAN
	// Perform byte order swap for loaded fields, so getPoint() doesn't need to do it.
	if (buffer != nullptr) {
		for (uint32_t i = 0; i < size; i++) {
			ImageStreamPoint pt = buffer[i];
			pt.offset = le64toh(pt.offset);
			pt.length = le32toh(pt.length);
			buffer[i] = pt;
		}
	}
#endif
}

BevvyIndex::~BevvyIndex() {
	parent = nullptr;
}

uint32_t BevvyIndex::getBevvyID() const noexcept {
	return bevvyID;
}

uint64_t BevvyIndex::getDataOffset() const noexcept {
	return dataChunkOffset;
}

ImageStreamPoint BevvyIndex::getPoint(uint32_t offset) const noexcept {
	if (offset >= size || buffer == nullptr || parent == nullptr) {
#if DEBUG
		fprintf(aff4::getDebugOutput(), "%s[%d] : Unknown Point? %x \n", __FILE__, __LINE__, offset);
#endif
		ImageStreamPoint pt;
		pt.offset = 0;
		pt.length = 0;
		return pt;
	}
	return buffer[offset];
}

} /* namespace structs */
} /* namespace stream */
} /* namespace aff4 */
