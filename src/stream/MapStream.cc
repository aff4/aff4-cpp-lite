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

#include "MapStream.h"
#include "PortableEndian.h"

using namespace aff4::stream::structs;

namespace aff4 {
namespace stream {

MapStream::MapStream(const std::string& resource, aff4::container::AFF4ZipContainer* parent, uint64_t size,
		std::shared_ptr<aff4::IAFF4Stream>& unknownOverride, std::shared_ptr<aff4::IAFF4Stream>& mapGapStream) :
		AFF4Resource(resource), parent(parent), closed(false), length(size) {

	// Add default properties for this stream type.
	addProperty(aff4::Lexicon::AFF4_TYPE, aff4::rdf::RDFValue(aff4::Lexicon::AFF4_IMAGESTREAM_TYPE));
	addProperty(aff4::Lexicon::AFF4_SIZE, aff4::rdf::RDFValue((int64_t) length));

	initStreamVector(unknownOverride);
	initMap(mapGapStream);
	if (length == 0) {
		close();
	}
}

MapStream::~MapStream() {
	close();
}

uint64_t MapStream::size() noexcept {
	return length;
}

void MapStream::close() noexcept {
	if (!closed.exchange(true)) {
		parent = nullptr;
		streams.clear();
		externalContainers.clear();
	}
}

int64_t MapStream::read(void *buf, uint64_t count, uint64_t offset) noexcept {
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
//	fprintf( stderr, "%s[%d] : Reading  %" PRIu64 " : %" PRIu64 " \n", __FILE__, __LINE__, offset, count);
	fprintf( aff4::getDebugOutput(), "%s[%d] : Reading  %" PRIx64 " : %" PRIx64 " \n", __FILE__, __LINE__, offset, count);
#endif

	uint64_t leftToRead = count;
	uint64_t actualRead = 0;
	uint8_t* buffer = static_cast<uint8_t*>(buf);
	MapEntryPoint entry;

	while (leftToRead > 0) {
		// Get the map entry for the current offset;

		/*
		 * map.lower_bound() will match either exact entry (offset = key), or get the next entry greater than offset.
		 * if offset != key, we need the entry prior, hence the check of entry offset.
		 */
		auto mapIt = map.lower_bound(offset);
		if (mapIt != map.end()) {
			entry = mapIt->second;
			if (entry.offset > offset) {
				// We have gone 1 entry too far, so get the prior map entry.
				mapIt--;
				entry = mapIt->second;
			}
		} else {
			// past last, get last entry.
			auto endIt = map.rbegin();
			entry = endIt->second;
		}

		std::shared_ptr<aff4::IAFF4Stream> stream = streams[entry.streamID];
		// Offset into the lower stream
		uint64_t streamReadOffset = entry.streamOffset + (offset - entry.offset);
		uint64_t streadReadLength = std::min<uint64_t>(leftToRead, (entry.length - (offset - entry.offset)));
		int64_t res = stream->read(buffer, streadReadLength, streamReadOffset);
		if (res <= 0) {
			// fail it.
			return -1;
		}
		actualRead += streadReadLength;
		offset += streadReadLength;
		leftToRead -= streadReadLength;
		buffer += streadReadLength;
	}
	return actualRead;
}

void MapStream::initStreamVector(std::shared_ptr<aff4::IAFF4Stream>& unknownOverride) {
	streams.clear();
	// Get a ZipSegmentStream and compare the results.
	std::string segmentName = getResourceID() + "/idx";
	std::shared_ptr<aff4::IAFF4Stream> stream = parent->getSegment(segmentName);
	if (stream == nullptr) {
		return;
	}
	std::unique_ptr<char[]> bufferIDX(new char[stream->size()]);
	int64_t res = stream->read(bufferIDX.get(), stream->size(), 0);
	stream->close();
	if (res > 0) {
		// convert the buffer into a string...
		std::string idx(bufferIDX.get(), res);

		std::stringstream data(idx);
		std::string line;

		while (std::getline(data, line, '\n')) {
			if (!line.empty()) {
#if DEBUG
				fprintf( aff4::getDebugOutput(), "%s[%d] : Stream : %s\n", __FILE__, __LINE__, line.c_str());
#endif
				std::shared_ptr<IAFF4Stream> stream;
				if ((line.compare(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_UNKNOWN)) == 0)
						&& (unknownOverride != nullptr)) {
					stream = unknownOverride;
				} else {
					stream = parent->getImageStream(line);
				}
				// We have this stream from the parent container
				if (stream != nullptr) {
					streams.push_back(stream);
				} else {
					// look at the resolver for this stream.
					stream = queryResolver(parent->getResolver(), line);
					if (stream != nullptr) {
						streams.push_back(stream);
					} else {
						// Nothing from the resolver.
						streams.push_back(aff4::stream::createUnknownStream(line));
					}
				}
			}
		}
	}
}

void MapStream::initMap(std::shared_ptr<aff4::IAFF4Stream>& mapGapStream) {
	map.clear();
	std::string segmentName = getResourceID() + "/map";
	std::shared_ptr<aff4::IAFF4Stream> stream = parent->getSegment(segmentName);
	if (stream == nullptr) {
		// reset the stream length
		length = 0;
		return;
	}
	uint64_t streamSize = stream->size();
	std::unique_ptr<MapEntryPoint[]> buffer;
	uint64_t size = 0;
	if (streamSize > 0) {
		size = streamSize / sizeof(MapEntryPoint);
		buffer = std::unique_ptr<MapEntryPoint[]>(new MapEntryPoint[size]);
		stream->read(buffer.get(), streamSize, 0);
	}
	stream->close();
	if (buffer == nullptr) {
		// reset the stream length
		length = 0;
		return;
	}
	// Add the mapGapStream to the vector of streams.
	size_t mapGPSid = streams.size();
	streams.push_back(mapGapStream);

#if __BYTE_ORDER == __BIG_ENDIAN
	// Perform byte order swap for loaded fields, so we don't need to worry about it below.
	for (uint32_t i = 0; i < size; i++) {
		MapEntryPoint mapPoint = buffer[i];
		mapPoint.offset = le64toh(mapPoint.offset);
		mapPoint.length = le64toh(mapPoint.length);
		mapPoint.streamOffset = le64toh(mapPoint.streamOffset);
		mapPoint.streamID = le32toh(mapPoint.streamID);
		buffer[i] = mapPoint;
	}
#endif

	uint64_t offset = 0;
	// We have our map.
	for (uint32_t i = 0; i < size; i++) {
		MapEntryPoint mapPoint = buffer[i];
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : MapEntry : %" PRIx64" : %" PRIx64" : %" PRIx64" : %" PRIx32"\n",
				__FILE__, __LINE__, mapPoint.offset, mapPoint.length, mapPoint.streamOffset, mapPoint.streamID);
#endif
		if (offset != mapPoint.offset) {
			// Insert Sparse Point?

			MapEntryPoint sparseRegion;
			sparseRegion.offset = offset;
			sparseRegion.length = mapPoint.offset - offset;
			sparseRegion.streamOffset = offset;
			sparseRegion.streamID = (uint32_t)mapGPSid;
#if DEBUG
			fprintf( aff4::getDebugOutput(), "%s[%d] : Insert Sparse Region MapEntry : %" PRIx64" : %" PRIx64" : %" PRIx64" : %" PRIx32"\n",
					__FILE__, __LINE__, sparseRegion.offset, sparseRegion.length, sparseRegion.streamOffset, sparseRegion.streamID);
#endif
			map[offset] = sparseRegion;
			offset = mapPoint.offset;
		}
		if (mapPoint.streamID >= streams.size()) {
#if DEBUG
			fprintf( aff4::getDebugOutput(), "%s[%d] : Unknown Stream ID for : %" PRIx64" : %" PRIx64" : %" PRIx64" : %" PRIx32"\n",
					__FILE__, __LINE__, mapPoint.offset, mapPoint.length, mapPoint.streamOffset, mapPoint.streamID);
#endif
			// unknown entry.
			mapPoint.streamID = (uint32_t)streams.size();
			streams.push_back(aff4::stream::createUnknownStream());
		}
		map[offset] = mapPoint;
		offset += mapPoint.length;
	}
	if (offset < length) {
		// missing end?
		MapEntryPoint sparseRegion;
		sparseRegion.offset = offset;
		sparseRegion.length = size - offset;
		sparseRegion.streamOffset = offset;
		sparseRegion.streamID = (uint32_t)mapGPSid;
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : Insert Sparse Region MapEntry : %" PRIx64" : %" PRIx64" : %" PRIx64" : %" PRIx32"\n",
				__FILE__, __LINE__, sparseRegion.offset, sparseRegion.length, sparseRegion.streamOffset, sparseRegion.streamID);
#endif
		map[offset] = sparseRegion;
	}
	// If length not given then use the next expected offset as the stream length.
	if (length == 0) {
		length = offset;
	}

}

std::shared_ptr<aff4::IAFF4Stream> MapStream::queryResolver(aff4::IAFF4Resolver* resolver,
		const std::string& resource) {
	if (resolver == nullptr) {
		return nullptr;
	}
	// Our parent doesn't have it, so check the external resolver.
	if (resolver->hasResource(resource)) {
		std::shared_ptr<aff4::IAFF4Resource> res = resolver->open(resource);
		// see if our resource has a type property of ImageStream;
		if (res->getBaseType() == aff4::Lexicon::AFF4_IMAGESTREAM_TYPE) {
			std::shared_ptr<IAFF4Stream> stream = std::static_pointer_cast<IAFF4Stream>(res);
			return stream;
		}
	}
	// The resolver doesn't know it, so attempt to get it's parent.
	std::shared_ptr<aff4::rdf::Model> model = parent->getRDFModel();
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> object = model->getObjectInformation(resource);
	if (!object.empty()) {
		// We know of this at least.
		auto objIt = object.find(aff4::Lexicon::AFF4_STORED);
		if (objIt != object.end()) {
			std::vector<aff4::rdf::RDFValue> properties = objIt->second;
			if (!properties.empty()) {
				aff4::rdf::RDFValue value = properties[0];
				std::string parentResource = value.getValue();
				if (resolver->hasResource(parentResource)) {
					// resolver has the parent;
					std::shared_ptr<aff4::IAFF4Resource> res = resolver->open(parentResource);
					// see if our resource has a type property of ImageStream;
					if (res->getBaseType() == aff4::Lexicon::AFF4_ZIP_TYPE) {
						std::shared_ptr<aff4::IAFF4Container> container = std::static_pointer_cast<aff4::IAFF4Container>(res);
						externalContainers.push_back(container);
						if (container->hasResource(resource)) {
							std::shared_ptr<aff4::IAFF4Resource> childStream = container->open(resource);
							// see if our resource has a type property of ImageStream;
							if (childStream->getBaseType() == aff4::Lexicon::AFF4_IMAGESTREAM_TYPE) {
								std::shared_ptr<IAFF4Stream> stream = std::static_pointer_cast<IAFF4Stream>(
										childStream);
								return stream;
							}
						}
						return nullptr;

					}
				}
			}
		}
	}
	return nullptr;
}

/*
 * AFF4 Resource
 */

std::string MapStream::getResourceID() const noexcept {
	return AFF4Resource::getResourceID();
}

aff4::Lexicon MapStream::getBaseType() noexcept {
	return aff4::Lexicon::AFF4_IMAGESTREAM_TYPE;
}

std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> MapStream::getProperties() noexcept {
	return AFF4Resource::getProperties();
}

std::vector<aff4::rdf::RDFValue> MapStream::getProperty(aff4::Lexicon resource) noexcept {
	return AFF4Resource::getProperty(resource);
}

/*
* Internal API
*/

std::vector<std::shared_ptr<aff4::IAFF4Stream>>*  MapStream::getStreams() {
	return &streams;
}

std::map<uint64_t, aff4::stream::structs::MapEntryPoint>*  MapStream::getMap() {
	return &map;
}

}
/* namespace stream */
} /* namespace aff4 */
