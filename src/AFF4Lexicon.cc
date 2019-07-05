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

#include <map>

#include "aff4config.h"
#include "aff4.h"

namespace aff4 {

/*
 * Define all mappings.
 */
static const std::map<aff4::Lexicon, std::string> lexiconMappings = { //
		{ UNKNOWN, (AFF4_RDF_PREFIX "unknown") }, //
				{ AFF4_TYPE, (AFF4_RDF_PREFIX "type") }, //
				{ AFF4_STORED, (AFF4_BASE_URI "stored") }, //
				{ AFF4_CONTAINS, (AFF4_BASE_URI "contains") }, //
				{ AFF4_TARGET, (AFF4_BASE_URI "target") }, //
				{ AFF4_ZIP_TYPE, (AFF4_BASE_URI "ZipVolume") }, //
				{ AFF4_CREATION_TIME, (AFF4_BASE_URI "creationTime") }, //
				{ AFF4_IMAGE_TYPE, (AFF4_BASE_URI "Image") }, //
				{ AFF4_DISK_IMAGE_TYPE, (AFF4_BASE_URI "DiskImage") }, //
				{ AFF4_VOLUME_IMAGE_TYPE, (AFF4_BASE_URI "VolumeImage") }, //
				{ AFF4_MEMORY_IMAGE_TYPE, (AFF4_BASE_URI "MemoryImage") }, //
				{ AFF4_CONTIGUOUS_IMAGE_TYPE, (AFF4_BASE_URI "ContiguousImage") }, //
				{ AFF4_DISCONTIGUOUS_IMAGE_TYPE, (AFF4_BASE_URI "DiscontiguousImage") }, //
				{ AFF4_MAP_TYPE, (AFF4_BASE_URI "Map") }, //
				{ AFF4_MAP_GAP_STREAM, (AFF4_BASE_URI "mapGapDefaultStream") }, //
				{ AFF4_IMAGESTREAM_TYPE, (AFF4_BASE_URI "ImageStream") }, //
				{ AFF4_DATASTREAM, (AFF4_BASE_URI "dataStream") }, //
				{ AFF4_DEPENDENT_STREAM, (AFF4_BASE_URI "dependentStream") }, //
				{ AFF4_SIZE, (AFF4_BASE_URI "size") }, //
				{ AFF4_STREAM_CHUNK_SIZE, (AFF4_BASE_URI "chunkSize") }, //
				{ AFF4_STREAM_VERSION, (AFF4_BASE_URI "version") }, //
				{ AFF4_STREAM_CHUNKS_PER_SEGMENT, (AFF4_BASE_URI "chunksInSegment") }, //
				{ AFF4_IMAGE_COMPRESSION, (AFF4_BASE_URI "compressionMethod") }, //
				{ AFF4_IMAGE_COMPRESSION_ZLIB, ("https://www.ietf.org/rfc/rfc1950.txt") }, //
				{ AFF4_IMAGE_COMPRESSION_DEFLATE, ("https://tools.ietf.org/html/rfc1951") }, //
				{ AFF4_IMAGE_COMPRESSION_SNAPPY, ("http://code.google.com/p/snappy/") }, //
				{ AFF4_IMAGE_COMPRESSION_SNAPPY2, ("https://github.com/google/snappy") }, //
				{ AFF4_IMAGE_COMPRESSION_LZ4, ("https://code.google.com/p/lz4/") }, //
				{ AFF4_IMAGE_COMPRESSION_STORED, (AFF4_BASE_URI "NullCompressor") }, //
				{ AFF4_IMAGESTREAM_ZERO, (AFF4_BASE_URI "Zero") }, //
				{ AFF4_IMAGESTREAM_FF, (AFF4_BASE_URI "FFDevice") }, //
				{ AFF4_IMAGESTREAM_UNKNOWN, (AFF4_BASE_URI "UnknownData") }, //
				{ AFF4_IMAGESTREAM_UNREADABLE, (AFF4_BASE_URI "UnreadableData") }, //
				{ AFF4_IMAGESTREAM_SYMBOLIC_PREFIX, (AFF4_BASE_URI "SymbolicStream") }, //
				{ AFF4_MAP_IDX_HASH, (AFF4_BASE_URI  "mapIdxHash") }, //
				{ AFF4_MAP_POINT_HASH, (AFF4_BASE_URI "mapPointHash") }, //
				{ AFF4_MAP_PATH_HASH, (AFF4_BASE_URI "mapPathHash") }, //
				{ AFF4_MAP_HASH, (AFF4_BASE_URI  "mapHash") }, //
				{ AFF4_DIGEST_BLOCK_MAP_HASH_SHA512, (AFF4_BASE_URI "blockMapHashSHA512") }, //
				{ AFF4_ACQUISITION_COMPLETION_STATE, (AFF4_BASE_URI  "acquisitionCompletionState") }, //
				{ AFF4_ACQUISITION_TYPE, (AFF4_BASE_URI  "acquisitionType") }, //
				{ AFF4_HASH, (AFF4_BASE_URI "hash") }, //
				{ AFF4_DIGEST_MD5, (AFF4_BASE_URI "MD5") }, //
				{ AFF4_DIGEST_SHA1, (AFF4_BASE_URI "SHA1") }, //
				{ AFF4_DIGEST_SHA256, (AFF4_BASE_URI "SHA256") }, //
				{ AFF4_DIGEST_SHA512, (AFF4_BASE_URI "SHA512") }, //
				{ AFF4_DIGEST_BLAKE2B, (AFF4_BASE_URI "blake2b") }, //
				{ AFF4_DIGEST_BLAKE2B_, (AFF4_BASE_URI "Blake2b") }, //
				{ AFF4_PAGESIZE, (AFF4_BASE_URI "pageSize") }, //
				{ AFF4_MEMORY_PAGETABLE_ENTRY_OFFSET, (AFF4_BASE_URI "memoryPageTableEntryOffset") }, //
				{ AFF4_MEMORY_INSTALLED_SIZE, (AFF4_BASE_URI "memoryInstalledSize") }, //
				{ AFF4_MEMORY_ADDRESSIBLE_SIZE, (AFF4_BASE_URI "memoryAddressableSize") }, //
				{ AFF4_BLOCKSIZE, (AFF4_BASE_URI "blockSize") }, //
				{ AFF4_DISK_DEVICE_NAME, (AFF4_BASE_URI "diskDeviceName") }, //
				{ AFF4_DISK_DEVICE_FIRMWARE, (AFF4_BASE_URI "diskFirmware") }, //
				{ AFF4_DISK_DEVICE_INTERFACE, (AFF4_BASE_URI "diskInterfaceType") }, //
				{ AFF4_DISK_DEVICE_MAKE, (AFF4_BASE_URI "diskMake") }, //
				{ AFF4_DISK_DEVICE_MODEL, (AFF4_BASE_URI "diskModel") }, //
				{ AFF4_DISK_DEVICE_SERIAL, (AFF4_BASE_URI "diskSerial") }, //
				{ AFF4_DISK_DEVICE_SECTOR_COUNT, (AFF4_BASE_URI "sectorCount") }, //
				{ AFF4_CASE_DETAILS, (AFF4_BASE_URI "CaseDetails") }, //
				{ AFF4_CASE_DESCRIPTION, (AFF4_BASE_URI "caseDescription") }, //
				{ AFF4_CASE_NAME, (AFF4_BASE_URI "caseName") }, //
				{ AFF4_CASE_EXAMINER, (AFF4_BASE_URI "examiner") }, //

				{ AFF4_TOOL, (AFF4_BASE_URI "tool") }, //
				{ AFF4_MINOR_VERSION, (AFF4_BASE_URI "toolMinorVersion") }, //
				{ AFF4_MAJOR_VERSION, (AFF4_BASE_URI "toolMajorVersion") }, //

				/* Black Bag Technologies Custom Properties */
				{ BBT_APFS_CONTAINER_TYPE, (BBT_BASE_URI "APFSContainerType") }, //
				{ BBT_APFS_CONTAINER_TYPE_STANDARD , (BBT_BASE_URI "APFSStandardContainerType") }, //
				{ BBT_APFS_CONTAINER_TYPE_T2, (BBT_BASE_URI "APFST2ContainerType") }, //
				{ BBT_APFS_CONTAINER_TYPE_FUSION, (BBT_BASE_URI "APFSFusionContainerType") }, //
				{ BBT_CONTAINS_EXTENTS, (BBT_BASE_URI "ContainsExtents") }, //
				{ BBT_CONTAINS_UNALLOCATED, (BBT_BASE_URI "ContainsUnallocated") }, //
		};

namespace lexicon {

std::string getLexiconString(aff4::Lexicon lexicon) noexcept {
	auto it = lexiconMappings.find(lexicon);
	if (it != lexiconMappings.end()) {
		return it->second;
	}
	// Guaranteed to return something.
	return getLexiconString(Lexicon::UNKNOWN);
}

aff4::Lexicon getLexicon(const std::string& lexicon) noexcept {
	if (lexicon.length() == 0) {
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Unknown lexicon? %s\n", __FILE__, __LINE__, lexicon.c_str());
#endif
		return Lexicon::UNKNOWN;
	}
	for (auto it = lexiconMappings.begin(); it != lexiconMappings.end(); it++) {
		if (it->second.compare(lexicon.c_str()) == 0) {
			return it->first;
		}
	}
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Unknown lexicon? %s\n",  __FILE__, __LINE__,lexicon.c_str());
#endif
	return Lexicon::UNKNOWN;
}

} // namespace lexicon.

} // namespace aff4

