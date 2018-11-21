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
 * @file AFF4Image.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief aff4:Image object implementation.
 *
 * This class defines the default instance for aff4:Image objects.
 */

#ifndef SRC_IMAGE_AFF4IMAGE_H_
#define SRC_IMAGE_AFF4IMAGE_H_

#include "aff4config.h"
#include "aff4.h"

#include <atomic>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <memory>
#include <algorithm>

#include "Model.h"
#include "AFF4Resource.h"
#include "AFF4ZipContainer.h"
#include "AFF4Map.h"

#ifndef AFF4ZipContainer
namespace aff4 {
namespace container {
class AFF4ZipContainer;
}
}
#endif

namespace aff4 {
/**
 * @brief Base aff4:Image implementations.
 */
namespace image {

/**
 * @brief Base IAFF4Image implementation.
 */
class AFF4Image: public AFF4Resource, public IAFF4Image {
public:

	/**
	 * Create a AFF4 Image instance, backed by the given AFF4 Zip Container.
	 * @param resource The AFF4 Resource ID.
	 * @param parent The parent container.
	 */
	LIBAFF4_API_LOCAL AFF4Image(const std::string& resource, aff4::container::AFF4ZipContainer* parent);
	virtual ~AFF4Image();

	/*
	 * From AFF4Resource.
	 */
	std::string getResourceID() const noexcept;
	aff4::Lexicon getBaseType() noexcept;
	std::map<aff4::Lexicon, std::vector<aff4::rdf::RDFValue>> getProperties() noexcept;
	std::vector<aff4::rdf::RDFValue> getProperty(aff4::Lexicon resource) noexcept;

	/*
	 * From IAFF4Image
	 */
	std::shared_ptr<IAFF4Map> getMap() noexcept;

private:
	/**
	 * Parent container.
	 */
	aff4::container::AFF4ZipContainer* parent;
};

} /* namespace image */
} /* namespace aff4 */

#endif /* SRC_IMAGE_AFF4IMAGE_H_ */
