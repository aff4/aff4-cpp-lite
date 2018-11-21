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
 * @file AFF4Resource.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief Base Abstract Class for all AFF4 Object implementations
 *
 * This class provides the base methods for all AFF4 Objects. (All AFF4 Objects implement IAFF4Resource).
 */

#ifndef SRC_RESOURCE_AFF4RESOURCE_H_
#define SRC_RESOURCE_AFF4RESOURCE_H_

#include "IAFF4Resource.h"

namespace aff4 {

/**
 * @brief Base Abstract AFF4 Resource class.
 */
class AFF4Resource: public IAFF4Resource {
public:
	/**
	 * Create a new AFF4 Resource.
	 * <p>
	 * An empty property map will be created.
	 * @param resource The resource identifier
	 */
	explicit AFF4Resource(const std::string& resource) noexcept;
	/**
	 * Create a new AFF4 Resource with the given predefined aff4 properties map
	 * @param resource The resource identifier
	 * @param properties The predefined properites map.
	 */
	explicit AFF4Resource(const std::string& resource,
			std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>>& properties) noexcept;

	virtual ~AFF4Resource() {
	}

	/*
	 * From IAFF4Resource.
	 */

	std::string getResourceID() const noexcept;
	aff4::Lexicon getBaseType() noexcept;
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> getProperties() noexcept;
	std::vector<aff4::rdf::RDFValue> getProperty(aff4::Lexicon resource) noexcept;

	/*
	 * By default we just compare resource id.
	 */
	bool operator==(const AFF4Resource& rhs) {
		return (this->resource == rhs.resource);
	}

	bool operator!=(const AFF4Resource& rhs) {
		return !(*this == rhs);
	}

protected:
	/**
	 * Add the given property
	 * @param property The property
	 * @param values The collection of values.
	 */
	void addProperty(aff4::Lexicon property, std::vector<aff4::rdf::RDFValue>& values) noexcept;
	/**
	 * Add the given property
	 * @param property The property
	 * @param value The value.
	 */
	void addProperty(aff4::Lexicon property, aff4::rdf::RDFValue value) noexcept;

private:
	/**
	 * The aff4 resource.
	 */
	const std::string resource;
	/**
	 * The properties map.
	 */
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> properties;
};

} /* namespace aff4 */

#endif /* SRC_RESOURCE_AFF4RESOURCE_H_ */
