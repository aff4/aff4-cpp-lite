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

#include "AFF4Map.h"

namespace aff4 {
namespace map {

AFF4Map::AFF4Map(const std::string& resource, aff4::container::AFF4ZipContainer* parent) :
		AFF4Resource(resource), parent(parent), length(0), unknownOverride(nullptr), mapGapStreamOverride(nullptr) {
#if DEBUG_VERBOSE
	fprintf(aff4::getDebugOutput(), "%s[%d] : Create Map  %s \n", __FILE__, __LINE__, getResourceID().c_str());
#endif

	std::shared_ptr<aff4::rdf::Model> model = parent->getRDFModel();
	// Add information about THIS object to the object properties.
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> elements = model->getObjectInformation(resource);
	if (!elements.empty()) {
		for (auto it = elements.begin(); it != elements.end(); it++) {
			addProperty(it->first, it->second);
		}
	}

	/*
	 * Get the length of the map according to the RDF model.
	 */
	std::vector<aff4::rdf::RDFValue> values = getProperty(aff4::Lexicon::AFF4_SIZE);
	if (!values.empty()) {
		if (values[0].getXSDType() == aff4::rdf::XSDType::Long) {
			length = values[0].getLong();
		} else if (values[0].getXSDType() == aff4::rdf::XSDType::Int) {
			length = values[0].getInteger();
		} else if (values[0].getXSDType() == aff4::rdf::XSDType::String) {
			try {
				length = (int64_t)std::stoll(values[0].getValue());
			} catch (...){
				// ignore
			}
		}
	}
#if DEBUG_VERBOSE
	fprintf(aff4::getDebugOutput(), "%s[%d] : Map Length  %" PRIu64 " (%" PRIx64 ")\n", __FILE__, __LINE__, length, length);
#endif

	/*
	 * Get the map GapStream instance.
	 */
	values = getProperty(aff4::Lexicon::AFF4_MAP_GAP_STREAM);
	if (!values.empty()) {
		aff4::rdf::RDFValue v = values[0];
		if (v.getType() != aff4::Lexicon::UNKNOWN) {
			std::string mapGPS = aff4::lexicon::getLexiconString(v.getType());
			mapGapStreamOverride = parent->getImageStream(mapGPS);
		} else {
			mapGapStreamOverride = parent->getImageStream(values[0].getValue());
		}
	}
	// Set system default if nothing provided in the model.
	if (mapGapStreamOverride == nullptr) {
		mapGapStreamOverride = aff4::stream::createZeroStream();
	}
#if DEBUG_VERBOSE
	fprintf(aff4::getDebugOutput(), "%s[%d] : mapGapDefaultStream = %s\n", __FILE__, __LINE__, mapGapStreamOverride->getResourceID().c_str());
#endif
	values = getProperty(aff4::Lexicon::AFF4_DEPENDENT_STREAM);
	if (values.empty()) {
		/*
		 * No dependent streams provided. Load the idx file and extract from there.
		 */

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
			std::vector<aff4::rdf::RDFValue> streams;
			while (std::getline(data, line, '\n')) {
				if (!line.empty()) {
#if DEBUG_VERBOSE
					fprintf(aff4::getDebugOutput(), "%s[%d] : IDX Stream : %s\n", __FILE__, __LINE__, line.c_str());
#endif
					/*
					 * Check for symbolic stream types.
					 */
					if (resource.compare(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_ZERO)) == 0) {
						continue;
					}
					if (resource.compare(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_UNKNOWN)) == 0) {
						continue;
					}
					if (resource.compare(aff4::lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_UNREADABLE)) == 0) {
						continue;
					}
					if (aff4::util::hasPrefix(resource, lexicon::getLexiconString(aff4::Lexicon::AFF4_IMAGESTREAM_SYMBOLIC_PREFIX))) {
						continue;
					}
					aff4::rdf::RDFValue value(aff4::rdf::XSDType::Resource, aff4::Lexicon::AFF4_DEPENDENT_STREAM, line);
					streams.push_back(value);
				}
			}
			if (!streams.empty()) {
				addProperty(aff4::Lexicon::AFF4_DEPENDENT_STREAM, streams);
			}
		}
	}
}

AFF4Map::~AFF4Map() {
	parent = nullptr;
}

/*
 * IAFF4 Map
 */

uint64_t AFF4Map::size() const {
	return length;
}

std::shared_ptr<IAFF4Stream> AFF4Map::getStream() noexcept {
	std::lock_guard<std::recursive_mutex> lock(mapLock);
	/*
	 * Always generate a new map stream. Note: This is inline with the Java based reader as well.
	 */
	return std::make_shared<aff4::stream::MapStream>(getResourceID(), parent, length, unknownOverride,
			mapGapStreamOverride);
}

std::shared_ptr<IAFF4Stream> AFF4Map::getUnknownStreamOverride() noexcept {
	std::lock_guard<std::recursive_mutex> lock(mapLock);
	return unknownOverride;
}

void AFF4Map::setUnknownStreamOverride(std::shared_ptr<IAFF4Stream>& stream) noexcept {
	std::lock_guard<std::recursive_mutex> lock(mapLock);
	unknownOverride = stream;
}

std::shared_ptr<IAFF4Stream> AFF4Map::getMapGapStreamOverride() noexcept {
	std::lock_guard<std::recursive_mutex> lock(mapLock);
	return mapGapStreamOverride;
}

void AFF4Map::setMapGapStreamOverride(std::shared_ptr<IAFF4Stream>& stream) noexcept {
	std::lock_guard<std::recursive_mutex> lock(mapLock);
	mapGapStreamOverride = stream;
}

/*
 * AFF4 Resource
 */

std::string AFF4Map::getResourceID() const noexcept {
	return AFF4Resource::getResourceID();
}

aff4::Lexicon AFF4Map::getBaseType() noexcept {
	return aff4::Lexicon::AFF4_MAP_TYPE;
}

std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> AFF4Map::getProperties() noexcept {
	return AFF4Resource::getProperties();
}

std::vector<aff4::rdf::RDFValue> AFF4Map::getProperty(aff4::Lexicon resource) noexcept {
	return AFF4Resource::getProperty(resource);
}

} /* namespace map */
} /* namespace aff4 */
