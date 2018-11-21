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

#include "ImageStream.h"
#include <functional>
#include <inttypes.h>

namespace aff4 {
namespace stream {

ImageStream::ImageStream(const std::string& resource, aff4::container::AFF4ZipContainer* parent) :
		AFF4Resource(resource), parent(parent), closed(false), length(0), chunkSize(AFF4_DEFAULT_CHUNK_SIZE), chunksInSegment(
		AFF4_DEFAULT_CHUNKS_PER_SEGMENT) {

#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Create Image Stream  %s \n", __FILE__, __LINE__, getResourceID().c_str());
#endif

	// Add information about THIS object to the object properties.
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> elements = //
			this->parent->getRDFModel()->getObjectInformation(resource);

	if (!elements.empty()) {
		for (auto it = elements.begin(); it != elements.end(); it++) {
			addProperty(it->first, it->second);
		}
	} else {
		// set base type.
		addProperty(aff4::Lexicon::AFF4_TYPE, aff4::rdf::RDFValue(aff4::Lexicon::AFF4_IMAGESTREAM_TYPE));
		addProperty(aff4::Lexicon::AFF4_SIZE, aff4::rdf::RDFValue((int64_t) 0));
	}

	// Get the length according the RDF metadata.
	std::vector<aff4::rdf::RDFValue> values = getProperty((aff4::Lexicon::AFF4_SIZE));
	if (values.size() > 0) {
		if (values[0].getXSDType() == aff4::rdf::XSDType::Long) {
			length = values[0].getLong();
		} else {
			length = values[0].getInteger();
		}
	}
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Length %" PRIu64 " (%" PRIx64 ") \n", __FILE__, __LINE__, length, length);
#endif
	// Get the chunksize
	values = getProperty((aff4::Lexicon::AFF4_STREAM_CHUNK_SIZE));
	if (values.size() > 0) {
		if (values[0].getXSDType() == aff4::rdf::XSDType::Long) {
			chunkSize = (uint32_t)values[0].getLong();
		} else {
			chunkSize = values[0].getInteger();
		}
	}
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : ChunkSize  %" PRIu32 " (%" PRIx32 ")\n", __FILE__, __LINE__, chunkSize, chunkSize);
#endif
	// Get the chunksInSegments value.
	values = getProperty((aff4::Lexicon::AFF4_STREAM_CHUNKS_PER_SEGMENT));
	if (values.size() > 0) {
		if (values[0].getXSDType() == aff4::rdf::XSDType::Long) {
			chunksInSegment = (uint32_t)values[0].getLong();
		} else {
			chunksInSegment = values[0].getInteger();
		}
	}
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : ChunksInSegment  %" PRIu32 " (%" PRIx32 ") \n", __FILE__, __LINE__, chunksInSegment, chunksInSegment);
#endif
	// Get the compression algorithm
	values = getProperty((aff4::Lexicon::AFF4_IMAGE_COMPRESSION));
	if (values.size() > 0) {
		if (values[0].getXSDType() != aff4::rdf::XSDType::Resource) {
			std::string codecResource = values[0].getValue();
			codec = aff4::codec::getCodec(codecResource, chunkSize);
		} else {
			aff4::Lexicon codecResource = values[0].getType();
			codec = aff4::codec::getCodec(codecResource, chunkSize);
		}
	}

	if (codec == nullptr) {
		length = 0;
		close();
		return;
	}
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Compression  %s \n", __FILE__, __LINE__, codec->getResourceID().c_str());
#endif

	/**
	 * Set our bevvy index cache.
	 */
	bevvyLoader = std::unique_ptr<aff4::stream::structs::BevvyIndexLoader>(
			new aff4::stream::structs::BevvyIndexLoader(resource, parent));

	std::function<std::shared_ptr<aff4::stream::structs::BevvyIndex>(uint32_t)> bevvyLoaderFunction = std::bind(
			&aff4::stream::structs::BevvyIndexLoader::load, bevvyLoader.get(), std::placeholders::_1);

	bevvyIndexCache = std::make_shared<aff4::util::cache<uint32_t, std::shared_ptr<aff4::stream::structs::BevvyIndex>>>(
	AFF4_IMAGE_STREAM_BEVVY_INDEX_CACHE_SIZE, bevvyLoaderFunction);

	/**
	 * Set our data chunk cache.
	 */
	chunkLoader = std::unique_ptr<aff4::stream::structs::ChunkLoader>(
			new aff4::stream::structs::ChunkLoader(resource, parent, bevvyIndexCache, chunkSize, chunksInSegment,
					codec));

	std::function<cacheBuffer_t(uint64_t)> chunkLoaderFunction = std::bind(&aff4::stream::structs::ChunkLoader::load,
			chunkLoader.get(), std::placeholders::_1);

	// determine cache size;
	uint64_t cacheSize = aff4::stream::getImageStreamCacheSize() / chunkSize;
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Number of Chunk Cache Entries %" PRIu64 " \n", __FILE__, __LINE__, cacheSize);
#endif
	chunkCache = std::make_shared<aff4::util::cache<uint64_t, cacheBuffer_t>>(cacheSize, chunkLoaderFunction);
}

ImageStream::~ImageStream() {
	close();
}

uint64_t ImageStream::size() noexcept {
	return length;
}

void ImageStream::close() noexcept {
	if (!closed.exchange(true)) {
		parent = nullptr;
	}
}

/**
 * Floor the given offset to multiple of chunkSize.
 * @param offset The offset
 * @param size The chunkSize
 * @return The offset floored to multiple of chunkSize.
 */
inline uint64_t floor(uint64_t offset, uint64_t size) {
	return (offset / size) * size;
}

int64_t ImageStream::read(void *buf, uint64_t count, uint64_t offset) noexcept {
	if (closed) {
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

#if DEBUG
	//fprintf( stderr, "%s[%d] : Reading  %" PRIu64 " : %" PRIu64 " \n", __FILE__, __LINE__, offset, count);
#endif

	uint64_t leftToRead = count;
	uint64_t actualRead = 0;

	uint8_t* buffer = static_cast<uint8_t*>(buf);

	while (leftToRead > 0) {

		// Load our chunk.
		uint64_t chunkOffset = floor(offset, chunkSize);

		cacheBuffer_t entry = chunkCache->get(chunkOffset);
		if (entry.second == 0) {
			// failed to read.
			return -1;
		}
		uint64_t delta = offset - chunkOffset;
		uint8_t* source = entry.first.get() + delta;
		uint64_t toCopy = std::min(entry.second - delta, leftToRead);
		::memcpy(buffer, source, toCopy);

		actualRead += toCopy;
		offset += toCopy;
		leftToRead -= toCopy;
		buffer += toCopy;
	}
	return actualRead;
}

/*
 * AFF4 Resource
 */

std::string ImageStream::getResourceID() const noexcept {
	return AFF4Resource::getResourceID();
}

aff4::Lexicon ImageStream::getBaseType() noexcept {
	return aff4::Lexicon::AFF4_IMAGESTREAM_TYPE;
}

std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> ImageStream::getProperties() noexcept {
	return AFF4Resource::getProperties();
}

std::vector<aff4::rdf::RDFValue> ImageStream::getProperty(aff4::Lexicon resource) noexcept {
	return AFF4Resource::getProperty(resource);
}

} /* namespace stream */
} /* namespace aff4 */
