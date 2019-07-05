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
 * @file MapEntryPoint.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 Map Entry struct
 */

#ifndef SRC_STREAM_STRUCT_MAPENTRYPOINT_H_
#define SRC_STREAM_STRUCT_MAPENTRYPOINT_H_


#include "aff4config.h"
#include <stdint.h>

#ifndef PACKED_STRUCT
#ifdef _MSC_VER 
#define PACKED_STRUCT( __Declaration__ ) __pragma( pack(push, 1) ) struct __Declaration__ __pragma( pack(pop) ) 
#elif defined(__GNUC__) 
#define PACKED_STRUCT( __Declaration__ ) struct __Declaration__ __attribute__((__packed__)) 
#endif
#endif

namespace aff4 {
namespace stream {
namespace structs {

/**
 * @brief AFF4 Map Entry data type.
 */
PACKED_STRUCT(MapEntryPoint {

	/**
	 * The upper map offset
	 */
	uint64_t offset;
	/**
	 * The length.
	 */
	uint64_t length;
	/**
	 * The lower stream offset
	 */
	uint64_t streamOffset;
	/**
	 * The stream ID.
	 */
	uint32_t streamID;

});

/**
* Map Entry Compare Function.
*
* @param i The first map point.
* @param j The second map point.
* @return TRUE if the first map point is before the second map point.
*/
bool mapEntryPointCompare(const MapEntryPoint& i, const MapEntryPoint& j);

} /* namespace structs */
} /* namespace stream */
} /* namespace aff4 */




#endif /* SRC_STREAM_STRUCT_MAPENTRYPOINT_H_ */
