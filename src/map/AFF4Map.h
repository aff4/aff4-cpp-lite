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
 * @file AFF4Map.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief aff4:Map object implementation.
 *
 * This class defines the default instance for aff4:Map objects.
 */

#ifndef SRC_MAP_AFF4MAP_H_
#define SRC_MAP_AFF4MAP_H_

#include "aff4config.h"
#include "aff4.h"

#include <atomic>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <memory>
#include <algorithm>
#include <mutex>

#include "Model.h"
#include "AFF4Resource.h"
#include "AFF4ZipContainer.h"
#include "MapStream.h"

#ifndef AFF4ZipContainer
namespace aff4 {
namespace container {
class AFF4ZipContainer;
}
}
#endif

namespace aff4 {

/**
 * @brief Base aff4:Map implementations.
 */
namespace map {

/**
 * @brief Base IAFF4Map implementation.
 */
class AFF4Map: public AFF4Resource, public IAFF4Map {
public:

	/**
	 * Create a AFF4 Map instance, backed by the given AFF4 Zip Container.
	 * @param resource The AFF4 Resource ID.
	 * @param parent The parent container.
	 */
	LIBAFF4_API_LOCAL AFF4Map(const std::string& resource, aff4::container::AFF4ZipContainer* parent);
	virtual ~AFF4Map();

	/*
	 * From AFF4Resource.
	 */
	std::string getResourceID() const noexcept;
	aff4::Lexicon getBaseType() noexcept;
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> getProperties() noexcept;
	std::vector<aff4::rdf::RDFValue> getProperty(aff4::Lexicon resource) noexcept;

	/*
	 * From IAFF4Map
	 */
	uint64_t size() const ;
	std::shared_ptr<IAFF4Stream> getStream() noexcept;
	std::shared_ptr<IAFF4Stream> getUnknownStreamOverride() noexcept;
	void setUnknownStreamOverride(std::shared_ptr<IAFF4Stream>& stream) noexcept;
	std::shared_ptr<IAFF4Stream> getMapGapStreamOverride() noexcept;
	void setMapGapStreamOverride(std::shared_ptr<IAFF4Stream>& stream) noexcept;

private:
	/**
	 * Parent container.
	 */
	aff4::container::AFF4ZipContainer* parent;
	/**
	 * Size of stream that will be created
	 */
	uint64_t length;
	/**
	 * Lock of mapStream generation.
	 */
	std::recursive_mutex mapLock;
	/**
	 * Unknown Stream override
	 */
	std::shared_ptr<IAFF4Stream> unknownOverride;
	/**
	 * Map Gap Stream override
	 */
	std::shared_ptr<IAFF4Stream> mapGapStreamOverride;

};

} /* namespace map */
} /* namespace aff4 */

#endif /* SRC_MAP_AFF4MAP_H_ */
