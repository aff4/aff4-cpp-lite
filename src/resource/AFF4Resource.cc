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

#include "AFF4Resource.h"

namespace aff4 {

AFF4Resource::AFF4Resource(const std::string& resource) noexcept //
		:resource(resource) {
}

AFF4Resource::AFF4Resource(const std::string& resource,
		std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>>& properties) noexcept //
				:resource(resource), properties(properties) {
}

std::string AFF4Resource::getResourceID() const noexcept {
	return resource;
}

std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> AFF4Resource::getProperties() noexcept {
	return properties;
}

aff4::Lexicon AFF4Resource::getBaseType() noexcept {
	std::vector<aff4::rdf::RDFValue> properties = getProperty(aff4::Lexicon::AFF4_TYPE);
	if (!properties.empty()) {
		for (aff4::rdf::RDFValue v : properties) {
			if (v.getType() != aff4::Lexicon::UNKNOWN) {
				return v.getType();
			}
		}
	}
	return aff4::Lexicon::UNKNOWN;
}

std::vector<aff4::rdf::RDFValue> AFF4Resource::getProperty(aff4::Lexicon resource) noexcept {
	auto it = properties.find(resource);
	if (it != properties.end()) {
		return it->second;
	}
	// Empty vector.
	return std::vector<aff4::rdf::RDFValue>();
}

void AFF4Resource::addProperty(aff4::Lexicon property, std::vector<aff4::rdf::RDFValue>& values) noexcept {
	properties[property] = values;
}

void AFF4Resource::addProperty(aff4::Lexicon property, aff4::rdf::RDFValue value) noexcept {
	std::vector<aff4::rdf::RDFValue> values;
	values.push_back(value);
	addProperty(property, values);
}

} /* namespace aff4 */
