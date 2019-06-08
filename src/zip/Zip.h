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
 * @file Zip.h
 * @author Schatz Forensic, Ptd Ltd.
 * @version 1.0
 * @date 12-Sep-2017
 * @copyright Copyright Schatz Forensic, Ptd Ltd. 2017. All Rights Reserved. This project is released under the LGPL 3.0+.
 *
 * @brief AFF4 Zip Reader implementation.
 *
 * This class an interface with interacting with Zip files for the AFF4 reader.
 */

#ifndef SRC_ZIP_ZIP_H_
#define SRC_ZIP_ZIP_H_

#include "aff4config.h"
#include "aff4.h"

#include <zlib.h>
#include <atomic>
#include <cstdio>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <fcntl.h>
#include <cerrno>

namespace aff4 {
/**
 * @brief AFF4 Zip reader implementation.
 */
namespace zip {

/**
 * The Zip segment is stored. (no compression)
 */
#define ZIP_STORED 0
/**
 * The Zip segment is compressed with DEFLATE. (Not supported)
 */
#define ZIP_DEFLATE 8
/**
 * The buffer size for finding the EOCD of the zip file.
 */
#define AFF4_ZIP_BUFFER_SIZE 4096

/**
 * This is the largest file size which may be represented by a regular zip file without using Zip64 extensions.
 */
#define  ZIP64_LIMIT ((1LL << 31)-1)

#ifndef PACKED_STRUCT
#ifdef _MSC_VER 
#define PACKED_STRUCT( __Declaration__ ) __pragma( pack(push, 1) ) struct __Declaration__ __pragma( pack(pop) ) 
#elif defined(__GNUC__) 
#define PACKED_STRUCT( __Declaration__ ) struct __Declaration__ __attribute__((__packed__)) 
#endif
#endif

/**
 * @brief Common Struct definitions for Zip.
 */
namespace structs {

//! @cond Doxygen_Suppress

/** These are ZipFile structures */
PACKED_STRUCT(EndCentralDirectory {
	uint32_t magic = 0x6054b50;
	uint16_t number_of_this_disk = 0;
	uint16_t disk_with_cd = 0;
	uint16_t total_entries_in_cd_on_disk;
	uint16_t total_entries_in_cd;
	uint32_t size_of_cd = -1;
	uint32_t offset_of_cd = -1;
	uint16_t comment_len = 0;
});

/** As we parse these fields we populate the oracle */
PACKED_STRUCT(CDFileHeader {
	uint32_t magic = 0x2014b50;
	uint16_t version_made_by = 0x317;
	uint16_t version_needed = 0x14;
	uint16_t flags = 0x8;
	uint16_t compression_method;
	uint16_t dostime;
	uint16_t dosdate;
	uint32_t crc32_cs;
	int32_t compress_size = -1;
	int32_t file_size = -1;
	uint16_t file_name_length;
	uint16_t extra_field_len = 32;
	uint16_t file_comment_length = 0;
	uint16_t disk_number_start = 0;
	uint16_t internal_file_attr = 0;
	uint32_t external_file_attr = 0644 << 16L;
	uint32_t relative_offset_local_header = -1;
});

/**
 * Zip entry file header.
 */
PACKED_STRUCT(ZipFileHeader {
	uint32_t magic = 0x4034b50;
	uint16_t version = 0x14;
	uint16_t flags = 0x8;
	uint16_t compression_method;
	uint16_t lastmodtime;
	uint16_t lastmoddate;
	uint32_t crc32_cs;
	uint32_t compress_size;
	uint32_t file_size;
	uint16_t file_name_length;
	uint16_t extra_field_len;
});

/**
* Zip Data Descriptor 64 entry file header.
*/
PACKED_STRUCT(ZipDataDescriptor64 {
	uint32_t magic = 0x08074b50;
	uint32_t crc32_cs;
	uint64_t compress_size;
	uint64_t file_size;
});

/**
 * Zip64 File header
 */
PACKED_STRUCT(Zip64FileHeaderExtensibleField {
	uint16_t header_id = 1;
	uint16_t data_size = 28;
	uint64_t file_size;
	uint64_t compress_size;
	int64_t relative_offset_local_header;
	uint32_t disk_number_start = 0;
});

/**
 * Zip entry extra field.
 */
PACKED_STRUCT(ZipExtraFieldHeader {
	uint16_t header_id;
	uint16_t data_size;
});

/**
 * Zip64 End CD.
 */
PACKED_STRUCT(Zip64EndCD {
	uint32_t magic = 0x06064b50;
	uint64_t size_of_header = 0;
	uint16_t version_made_by = 0x2d;
	uint16_t version_needed = 0x2d;
	uint32_t number_of_disk = 0;
	uint32_t number_of_disk_with_cd = 0;
	uint64_t number_of_entries_in_volume;
	uint64_t number_of_entries_in_total;
	uint64_t size_of_cd;
	uint64_t offset_of_cd;
});

/**
 * Zip64 EOCD header.
 */
PACKED_STRUCT(Zip64CDLocator {
	uint32_t magic = 0x07064b50;
	uint32_t disk_with_cd = 0;
	uint64_t offset_of_end_cd;
	uint32_t number_of_disks = 1;
});

/**
 * Zip64 entry.
 */
PACKED_STRUCT(Zip64Entry {
	uint64_t size;
	uint64_t csize;
	uint64_t headerOffset;
	uint64_t dataOffset;
});

//! @endcond

} /* namespace structs */
/**
 * @brief Class representing a single segment within the Zip File.
 */
class ZipEntry {
public:
	/**
	 * A Zip Entry
	 * @param segmentName The name of the segment.
	 * @param headerOffset  The offset into the parent file which this segments header starts
	 * @param offset The offset into the parent file which this segments data starts
	 * @param length The uncompressed length of the segment
	 * @param compressedLength The compressed length of the segment
	 * @param compressionMethod The compression method to use if this segment is compressed.
	 */
	LIBAFF4_API ZipEntry(const std::string& segmentName, uint64_t headerOffset, uint64_t offset, uint64_t length,
			uint64_t compressedLength, int compressionMethod);
	virtual ~ZipEntry();

	/**
	 * Get the name of the segment.
	 * @return The name of the segment
	 */
	LIBAFF4_API std::string getSegmentName() const {
		return segmentName;
	}
	/**
	 * Get the offset of this header entry of the segment within the parent zip file.
	 * @return The offset of the header that makes up the segment
	 */
	LIBAFF4_API uint64_t getHeaderOffset() const {
		return headerOffset;
	}
	/**
	 * Get the offset of the segment within the parent zip file.
	 * @return The offset of the data that makes up the segment
	 */
	LIBAFF4_API uint64_t getOffset() const {
		return offset;
	}
	/**
	 * Get the (uncompressed) length of the segment
	 * @return The length
	 */
	LIBAFF4_API uint64_t getLength() const {
		return length;
	}
	/**
	 * Get the compressed length of the segment
	 * <p>
	 * This will typically be equal to the uncompresed length if the segment is stored.
	 * @return The compressed length
	 */
	LIBAFF4_API uint64_t getCompressedLength() const {
		return compressedLength;
	}

	/**
	 * Get the compression method.
	 * @return The compression method.
	 */
	LIBAFF4_API int getCompressionMethod() const {
		return compressionMethod;
	}

private:
	/**
	 * The name of the segment
	 */
	const std::string segmentName;
	/**
	 * The offset into the parent file which this segment header starts
	 */
	const uint64_t headerOffset;
	/**
	 * The offset into the parent file which this segment data starts
	 */
	const uint64_t offset;
	/**
	 *  The compressed length of the segment
	 */
	const uint64_t length;
	/**
	 *  The uncompressed length of the segment
	 */
	const uint64_t compressedLength;
	/**
	 *  The compression method to use if this segment is compressed.
	 */
	const int compressionMethod;
};

#ifndef ZipStream
class ZipStream;
#endif

/**
 * @brief Class representing a Zip container.
 */
class Zip {
public:
	/**
	 * Open an existing Zip Container.
	 * <p>
	 * On opening the Zip container will populate the entries vector. This this vector is empty, then consult errno for the error condition.
	 * @param filename The filename of the zip container.
	 */
	LIBAFF4_API explicit Zip(const std::string& filename);
	LIBAFF4_API virtual ~Zip();
	/**
	 * Close the underlying file for this zip container.
	 */
	LIBAFF4_API void close() noexcept;

	/**
	 * Get the filename of the zip container.
	 * @return The zip containers filename.
	 */
	LIBAFF4_API std::string getFilename() const noexcept ;

	/**
	 * Get the comment from the Zip container.
	 * @return The comment, or "" if no comment is present.
	 */
	LIBAFF4_API std::string getZipComment() const noexcept;
	/**
	 * Get a vector of all zip entries.
	 * @return A vector of all zip segments available in this zip file.
	 */
	LIBAFF4_API std::vector<std::shared_ptr<ZipEntry>> getEntries() const noexcept;

	/**
	 * Does the zip file contain the given segment?
	 * @param segmentName The name of the segment
	 * @return TRUE if the Zip container has this segment.
	 */
	LIBAFF4_API bool hasEntry(const std::string& segmentName) const noexcept;
	/**
	 * Create a readable stream for the given segment name
	 * @param segmentName The name of the segment to open
	 * @return A Stream, or NULL if segment doesn't exist or is unreadable.
	 */
	LIBAFF4_API std::shared_ptr<IAFF4Stream> getStream(const std::string& segmentName) noexcept;

	/**
	 * Read a number of bytes from the stream starting at offset
	 * @param buf A pointer to the buffer to read to.
	 * @param count The number of bytes to read
	 * @param offset The offset from the start of the stream.
	 * @return The number of bytes read. (0 indicates nothing read, or -1 indicates error.
	 */
	LIBAFF4_API int64_t fileRead(void *buf, uint64_t count, uint64_t offset) noexcept;
private:
	/**
	 * The filename of the zip container.
	 */
	std::string filename;
	/**
	 * File Handle
	 */
#ifdef _WIN32
	HANDLE fileHandle;
#else
	int fileHandle;
#endif
	/**
	 * file length.
	 */
	uint64_t length;
	/**
	 * Is this container closed.
	 */
	std::atomic<bool> closed;
	/**
	 * vector of all entries.
	 */
	std::vector<std::shared_ptr<ZipEntry>> entries;
	/**
	 * The Zip Comment from the EOCD.
	 */
	std::string comment;

	/**
	 * Attempt to find the Central Directory, and construct a vector of ZipEntry.
	 */
	LIBAFF4_API_LOCAL void parseCD() noexcept;
};

} /* namespace zip */
} /* namespace aff4 */

#endif /* SRC_ZIP_ZIP_H_ */
