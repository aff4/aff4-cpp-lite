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

#include "aff4config.h"
#include "aff4.h"

/**
 * The default cache size for reads.
 */
static uint64_t CHUNK_CACHE_SIZE = AFF4_IMAGE_STREAM_CHUNK_CACHE_SIZE;

/**
 * The default output for debug output.
 */
static FILE* debugOutput = stderr;

std::string aff4::getVersionString() noexcept {
	return std::string(PACKAGE_STRING);
}

void aff4::setDebugOutput(FILE* fileHandle) {
	debugOutput = fileHandle;
}

FILE* aff4::getDebugOutput() {
	return debugOutput;
}

uint64_t aff4::stream::getImageStreamCacheSize() {
	return CHUNK_CACHE_SIZE;
}

uint64_t aff4::stream::setImageStreamCacheSize(uint64_t size) {
	uint64_t oldValue = CHUNK_CACHE_SIZE;
	// Check for greater or equal 1 MiB.
	if (size >= AFF4_MINIMUM_IMAGE_STREAM_CHUNK_CACHE_SIZE) {
		// Check for power 2
		if (!(size & (size - 1))) {
			CHUNK_CACHE_SIZE = size;
		}
	}
	return oldValue;
}
