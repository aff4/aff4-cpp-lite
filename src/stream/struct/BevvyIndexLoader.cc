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

#include "BevvyIndexLoader.h"

namespace aff4 {
namespace stream {
namespace structs {

BevvyIndexLoader::BevvyIndexLoader(const std::string& resource, aff4::container::AFF4ZipContainer* parent) :
		resource(resource), parent(parent) {
}

BevvyIndexLoader::~BevvyIndexLoader() {
	parent = nullptr;
}

std::shared_ptr<BevvyIndex> BevvyIndexLoader::load(uint32_t bevvyID) {
	return std::make_shared<BevvyIndex>(resource, bevvyID, parent);
}

} /* namespace structs */
} /* namespace stream */
} /* namespace aff4 */
