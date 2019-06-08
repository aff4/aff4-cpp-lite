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

#include "ZipStream.h"

namespace aff4 {
namespace stream {

#define ZIP_WHOLE_STREAM 32 * 1024 * 1024

ZipSegmentStream::ZipSegmentStream(const std::string& resource, std::shared_ptr<aff4::zip::ZipEntry> entry,	aff4::zip::Zip* container) :
		AFF4Resource(resource), closed(false), entry(entry), container(container) {

	addProperty(aff4::Lexicon::AFF4_TYPE, aff4::rdf::RDFValue(aff4::Lexicon::AFF4_IMAGESTREAM_TYPE));
	addProperty(aff4::Lexicon::AFF4_SIZE, aff4::rdf::RDFValue((int64_t) entry->getLength()));
}

ZipSegmentStream::~ZipSegmentStream() {
	close();
}

uint64_t ZipSegmentStream::size() noexcept {
	if (entry.get() == nullptr) {
		return 0;
	}
	return entry->getLength();
}

void ZipSegmentStream::close() noexcept {
	if (!closed.exchange(true)) {
		// Clear entries and buffers.
		entry = nullptr;
		container = nullptr;
	}
}

int64_t ZipSegmentStream::read(void *buf, uint64_t count, uint64_t offset) noexcept {
	if (closed) {
		return -1;
	}
	if (entry.get() == nullptr) {
		errno = EPERM;
		return -1;
	}
	// If offset beyond end, return.
	if (offset > size()) {
		return 0;
	}
	// If offset + count, will go beyond end, truncate count.
	if (offset + count > size()) {
		count -= ((offset + count) - size());
	}
	if (entry->getCompressionMethod() == ZIP_DEFLATE){
		return readCompressed(buf, count, offset);
	}
	if (entry->getCompressionMethod() != ZIP_STORED) {
		// We don't yet support other compression methods.
		errno = EPERM;
		return -1;
	}
	return container->fileRead(buf, count, offset + entry->getOffset());
}

/*
 * AFF4 Resource
 */

std::string ZipSegmentStream::getResourceID() const noexcept {
	return AFF4Resource::getResourceID();
}

aff4::Lexicon ZipSegmentStream::getBaseType() noexcept {
	return aff4::Lexicon::AFF4_IMAGESTREAM_TYPE;
}

std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> ZipSegmentStream::getProperties() noexcept {
	return AFF4Resource::getProperties();
}

std::vector<aff4::rdf::RDFValue> ZipSegmentStream::getProperty(aff4::Lexicon resource) noexcept {
	return AFF4Resource::getProperty(resource);
}

/*
 * Compressed stream helpers.
 */

int64_t ZipSegmentStream::readCompressed(void *buf, uint64_t count, uint64_t offset) noexcept {

	// If the size of the stream is less than 32MB, take a poor mans implementation.
	if (size() <= ZIP_WHOLE_STREAM) {
		/*
		 * Poor mans implementation - decompress the whole lot and copy result buffer.
		 */
		std::shared_ptr<uint8_t> buffer(new uint8_t[entry->getCompressedLength()], std::default_delete<uint8_t[]>());
		std::shared_ptr<uint8_t> decompbuffer(new uint8_t[entry->getLength()], std::default_delete<uint8_t[]>());
		int64_t read = container->fileRead(buffer.get(), entry->getCompressedLength(), entry->getOffset());
		if (read == -1) {
			return read;
		}
		// Decompress.
		z_stream zstream;
		::memset(&zstream, 0, sizeof(zstream));
		zstream.next_in = buffer.get();
		zstream.avail_in = read;
		zstream.next_out = decompbuffer.get();
		zstream.avail_out = entry->getLength();

		if (inflateInit2(&zstream, -15) != Z_OK) {
			return -1;
		}

		if (inflate(&zstream, Z_FINISH) != Z_STREAM_END) {
			inflateEnd(&zstream);
			return -1;
		}

		inflateEnd(&zstream);
		// copy data to result buffer.
		::memcpy(buf, decompbuffer.get() + offset, count);
		return count;

	} else {
		// greater than 32MB

		// FIXME: IMPLEMENT.
	}
	// failed?
	return -1;
}

} /* namespace zip */
} /* namespace aff4 */
