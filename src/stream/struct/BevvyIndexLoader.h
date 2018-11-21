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
 * @file BevvyIndexLoader.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 Bevvy Index Loader
 *
 * This class provides function methods for the cache loader.
 */
#ifndef SRC_STREAM_STRUCT_BEVVYINDEXLOADER_H_
#define SRC_STREAM_STRUCT_BEVVYINDEXLOADER_H_


#include "aff4config.h"
#include "aff4.h"

#include "AFF4ZipContainer.h"
#include "BevvyIndex.h"

#ifndef AFF4ZipContainer
namespace aff4 {
namespace container {
class AFF4ZipContainer;
}
}
#endif

#ifndef BevvyIndex
namespace aff4 {
namespace stream {
namespace structs {
class BevvyIndex;
}
}
}
#endif

namespace aff4 {
namespace stream {
/**
 * @brief AFF4 Struct Definitions and Object loaders.
 */
namespace structs {

/**
 * @brief AFF4 Bevvy Index Loader (cache).
 */
class BevvyIndexLoader {
public:
	/**
	 * Create new bevvy index loader.
	 * @param resource The resource id of the image stream we are servicing.
	 * @param parent The parent container which can service load requests.
	 */
	LIBAFF4_API_LOCAL BevvyIndexLoader(const std::string& resource, aff4::container::AFF4ZipContainer* parent);
	virtual ~BevvyIndexLoader();

	/**
	 * Load the given Bevvy Index instance
	 * @param bevvyID The beevy ID to load
	 * @return A pointer to the created BevvyIndex.
	 */
	LIBAFF4_API_LOCAL std::shared_ptr<BevvyIndex> load(uint32_t bevvyID);

private:
	/**
	 * The resource of the image stream we are servicing.
	 */
	std::string resource;
	/**
	 * The parent container instance.
	 */
	aff4::container::AFF4ZipContainer* parent;
};

} /* namespace structs */
} /* namespace stream */
} /* namespace aff4 */

#endif /* SRC_STREAM_STRUCT_BEVVYINDEXLOADER_H_ */
