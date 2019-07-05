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

#include "AFF4Image.h"

namespace aff4 {
namespace image {

AFF4Image::AFF4Image(const std::string& resource, aff4::container::AFF4ZipContainer* parent) :
		AFF4Resource(resource), parent(parent) {
#if DEBUG
	fprintf(aff4::getDebugOutput(), "%s[%d] : Create Image? %s\n", __FILE__, __LINE__, resource.c_str());
#endif
	std::shared_ptr<aff4::rdf::Model> model = parent->getRDFModel();
	// Add information about THIS object to the object properties.
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> elements = model->getObjectInformation(resource);
	if (!elements.empty()) {
		for (auto it = elements.begin(); it != elements.end(); it++) {
			addProperty(it->first, it->second);
		}
	}
	// Also look for aff4::CaseDetails where aff4:target == us.
	std::vector<std::string> resources = model->getResourcesOfType(aff4::Lexicon::AFF4_CASE_DETAILS);
	for (std::string res : resources) {
		std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> elements = model->getObjectInformation(res);
		if (!elements.empty()) {
			for (auto it = elements.begin(); it != elements.end(); it++) {
				if (it->first == aff4::Lexicon::AFF4_TARGET) {
					std::vector<aff4::rdf::RDFValue> values = it->second;
					for (aff4::rdf::RDFValue v : values) {
						if ((v.getXSDType() == aff4::rdf::XSDType::Resource) && v.getValue() == resource) {
							// The aff4:target property is us!
							for (auto it2 = elements.begin(); it2 != elements.end(); it2++) {
								if (it2->first == aff4::Lexicon::AFF4_CASE_NAME) {
									addProperty(it2->first, it2->second);
								} else if (it2->first == aff4::Lexicon::AFF4_CASE_DESCRIPTION) {
									addProperty(it2->first, it2->second);
								} else if (it2->first == aff4::Lexicon::AFF4_CASE_EXAMINER) {
									addProperty(it2->first, it2->second);
								}
							}
							break;
						}
					}
				}
			}
		}
	}
}

AFF4Image::~AFF4Image() {
	// NOP.
}

/*
 * IAFF4Image.
 */

std::shared_ptr<aff4::IAFF4Map> AFF4Image::getMap() noexcept {
	// Find our dependentStream instance.
#if DEBUG
	fprintf(aff4::getDebugOutput(), "%s[%d] : Open Map for Image. %s\n", __FILE__, __LINE__, getResourceID().c_str());
#endif
	std::vector<aff4::rdf::RDFValue> values = getProperty(aff4::Lexicon::AFF4_DATASTREAM);
	if (!values.empty()) {
		aff4::rdf::RDFValue v = values[0];
		std::string resource = v.getValue();
		if (!resource.empty()) {
			return std::make_shared<aff4::map::AFF4Map>(resource, parent);
		}
	}
	// Recheck to see if we are a aff4:Map?
	values = getProperty(aff4::Lexicon::AFF4_TYPE);
	if (!values.empty()) {
		for (aff4::rdf::RDFValue v : values) {
			if (v.getType() == aff4::Lexicon::AFF4_MAP_TYPE) {
				return std::make_shared<aff4::map::AFF4Map>(getResourceID(), parent);
			}
		}
	}
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : No Map Stream found for %s \n", __FILE__, __LINE__, getResourceID().c_str());
#endif
	return nullptr;
}

/*
 * AFF4 Resource
 */

std::string AFF4Image::getResourceID() const noexcept {
	return AFF4Resource::getResourceID();
}

aff4::Lexicon AFF4Image::getBaseType() noexcept {
	return aff4::Lexicon::AFF4_IMAGE_TYPE;
}

std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> AFF4Image::getProperties() noexcept {
	return AFF4Resource::getProperties();
}

std::vector<aff4::rdf::RDFValue> AFF4Image::getProperty(aff4::Lexicon resource) noexcept {
	return AFF4Resource::getProperty(resource);
}

} /* namespace image */
} /* namespace aff4 */
