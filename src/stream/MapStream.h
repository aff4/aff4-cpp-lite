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
 * @file MapStream.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 Map Stream implementation
 *
 * This class provides implementation of aff4:ImageStream backed by a AFF4 Map instance.
 */

#ifndef SRC_STREAM_MAPSTREAM_H_
#define SRC_STREAM_MAPSTREAM_H_

#include "aff4config.h"
#include "aff4.h"

#include <atomic>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <inttypes.h>

#include "AFF4Resource.h"
#include "AFF4ZipContainer.h"
#include "AFF4Lexicon.h"
#include "MapEntryPoint.h"

#ifndef AFF4ZipContainer
namespace aff4 {
namespace container {
class AFF4ZipContainer;
}
}
#endif


namespace aff4 {
namespace stream {

/**
 * @brief Base AFF4 Map Stream.
 */
class MapStream: public AFF4Resource, public IAFF4Stream {
public:

	/**
	 * Create a new map based stream
	 * @param resource The resource of the map
	 * @param parent The parent container
	 * @param size The expected size of the map stream
	 * @param unknownOverride Override for the unknown stream (may be null).
	 * @param mapGapStream Stream to use for map gap stream
	 */
	LIBAFF4_API_LOCAL MapStream(const std::string& resource, aff4::container::AFF4ZipContainer* parent, uint64_t size,
			std::shared_ptr<aff4::IAFF4Stream>& unknownOverride, std::shared_ptr<aff4::IAFF4Stream>& mapGapStream);
	virtual ~MapStream();

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

	/*
	* Internal API
	*/

	/**
	* Get the vector of streams used to service the map.
	* <p>
	* <b>THIS IS INTERNAL API AND MAY CHANGE AT ANY TIME</b>
	*
	* @return A pointer to the internal vector of stream instances.
	*/
	LIBAFF4_API std::vector<std::shared_ptr<aff4::IAFF4Stream>>* getStreams();
	/**
	* Get the map.
	* <p>
	* <b>THIS IS INTERNAL API AND MAY CHANGE AT ANY TIME</b>
	*
	* @return A pointer to the internal map.
	*/
	LIBAFF4_API std::map<uint64_t, aff4::stream::structs::MapEntryPoint>* getMap();

private:
	/**
	 * Parent container.
	 */
	aff4::container::AFF4ZipContainer* parent;
	/**
	 * Closed flag
	 */
	std::atomic<bool> closed;
	/**
	 * The length of the stream.
	 */
	uint64_t length;
	/**
	 * Vector of streams used by this map.
	 */
	std::vector<std::shared_ptr<aff4::IAFF4Stream>> streams;
	/**
	 * Map of entries. (upper map offset = map point entry(lower stream)).
	 */
	std::map<uint64_t, aff4::stream::structs::MapEntryPoint> map;
	/**
	 * Collection of external containers for streams not contained in parent;
	 */
	std::vector<std::shared_ptr<aff4::IAFF4Container>> externalContainers;
	/**
	 * Read the idx file and create the vector of streams.
	 *
	 * This will also attempt to look for stream using the parent and resolver if present.
	 *
	 * @param unknownOverride The stream to use to override the set Unknown Stream if used.
	 */
	void initStreamVector(std::shared_ptr<aff4::IAFF4Stream>& unknownOverride);
	/**
	 * Read the map file and create the map of streams for this map instance.
	 *
	 * This will also attempt to sanitise any input (eg missing streams).
	 * @param mapGapStream The stream to use to fill in sparse regions in the map.
	 */
	void initMap(std::shared_ptr<aff4::IAFF4Stream>& mapGapStream);

	/**
	 * Query an external resolver for this resource.
	 *
	 * @param resolver The resolver to use.
	 * @param resource The resource to query for.
	 * @return The AFF4 object requested or NULL if not found
	 */
	std::shared_ptr<aff4::IAFF4Stream> queryResolver(aff4::IAFF4Resolver* resolver, const std::string& resource);
};

} /* namespace stream */
} /* namespace aff4 */

#endif /* SRC_STREAM_MAPSTREAM_H_ */
