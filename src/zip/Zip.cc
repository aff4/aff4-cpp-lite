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

#include "Zip.h"
#include "ZipStream.h"
#include <inttypes.h>
#include "PortableEndian.h"
#include "StringUtil.h"

/*
 * Handle some of the macOS vs Linux
 */

#if defined(__APPLE__)

// O_LARGEFILE is always on for macOS and thus not defined.
#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

// The following are equal on macOS
#define lseek64 lseek
#define pread64 pread

#endif

namespace aff4 {
namespace zip {

ZipEntry::ZipEntry(const std::string& segmentName, uint64_t headerOffset, uint64_t offset, uint64_t length,
		uint64_t uncompressedLength, int compressionMethod) :
		segmentName(segmentName), headerOffset(headerOffset), offset(offset), length(length), uncompressedLength(
				uncompressedLength), compressionMethod(compressionMethod) {
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : ZipEntry : %s %" PRIu64 " : %" PRIu64 "\n", __FILE__, __LINE__, segmentName.c_str(),
			offset, uncompressedLength);
#endif
}

ZipEntry::~ZipEntry() {
	// NOP.
}

Zip::Zip(const std::string& filename) :
		filename(filename), fileHandle(0), length(0), closed(true), comment("") {

#ifndef _WIN32
	/*
	* POSIX based systems.
	*/
	fileHandle = ::open(filename.c_str(), O_RDONLY | O_LARGEFILE);
	if (fileHandle == -1) {
		// we failed, so return nothing. (error will be in errno).
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : Unable to open Zip : %s \n", __FILE__, __LINE__, filename.c_str());
#endif
		return;
	}
	length = ::lseek64(fileHandle, 0, SEEK_END);
	::lseek64(fileHandle, 0, SEEK_SET);

#else
	/*
	* Windows based systems
	*/
	// Note: DO NOT ADD OVERLAPPED ATTRIBUTE for opening the file.
	std::wstring wpath = aff4::util::s2ws(filename);
	fileHandle = CreateFile(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
	if (fileHandle == INVALID_HANDLE_VALUE) {
#if DEBUG
		fprintf(aff4::getDebugOutput(), "%s[%d] : Unable to open Zip : %s \n", __FILE__, __LINE__, filename.c_str());
#endif
		return;
	}
	length = 0;
	LARGE_INTEGER plength;
	plength.QuadPart = 0;
	if (!GetFileSizeEx(fileHandle, &plength)) {
#if DEBUG
		fprintf(aff4::getDebugOutput(), "%s[%d] : Unable to get File Length of Zip : %s \n", __FILE__, __LINE__, filename.c_str());
#endif
		return;
	}
	length = plength.QuadPart;
	
#endif
	closed = false;
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Zip : %s : %" PRIu64 "\n", __FILE__, __LINE__, filename.c_str(), length);
#endif
	parseCD();
}

Zip::~Zip() {
	close();
}

void Zip::close() noexcept {
	if (!closed.exchange(true)) {
		entries.clear();
#ifndef _WIN32
		/*
		* POSIX based systems.
		*/
		::close(fileHandle);
#else 
		/*
		* Windows based systems
		*/
		CloseHandle(fileHandle);
#endif
	}
}

std::string Zip::getFilename() const noexcept {
	return filename;
}

std::string Zip::getZipComment() const noexcept {
	return comment;
}

std::vector<std::shared_ptr<ZipEntry>> Zip::getEntries() const noexcept {
	return entries;
}

bool Zip::hasEntry(const std::string& segmentName) const noexcept {
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Has Entry: %s \n", __FILE__, __LINE__, segmentName.c_str());
#endif
	// Find the Zip Entry.
	for (std::shared_ptr<ZipEntry> entry : entries) {
		if (entry->getSegmentName().compare(segmentName) == 0) {
			return true;
		}
	}
	return false;
}

std::shared_ptr<IAFF4Stream> Zip::getStream(const std::string& segmentName) noexcept {
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Zip Open Segment: %s \n", __FILE__, __LINE__, segmentName.c_str());
#endif
	// Find the Zip Entry.
	for (std::shared_ptr<ZipEntry> entry : entries) {
		if (entry->getSegmentName().compare(segmentName) == 0) {
			std::shared_ptr<aff4::stream::ZipSegmentStream> stream = std::make_shared<aff4::stream::ZipSegmentStream>(
					entry->getSegmentName(), entry, this);
#if DEBUG
			fprintf( aff4::getDebugOutput(), "%s[%d] : Zip Open Segment: %s Found: %" PRIu64 " : %" PRIu64 "\n", __FILE__, __LINE__,
					segmentName.c_str(), entry->getHeaderOffset(), entry->getUncompressedLength());
#endif
			return stream;
		}
	}
	// Unknown?
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Zip Open Segment: %s is UNKNOWN?\n", __FILE__, __LINE__, segmentName.c_str());
#endif
	std::shared_ptr<IAFF4Stream> s = nullptr;
	return s;
}

void Zip::parseCD() noexcept {
	errno = EIO;
	structs::EndCentralDirectory* endCD = nullptr;
	/* Find the End of Central Directory Record - We read about 4k of
	 data and scan for the header from the end, just in case there is
	 an archive comment appended to the end. */
	std::unique_ptr<uint8_t[]> buffer(new uint8_t[AFF4_ZIP_BUFFER_SIZE]);
	uint64_t read = AFF4_ZIP_BUFFER_SIZE;
	if (length < read) {
		read = length;
	}
	int64_t res = fileRead(buffer.get(), read, (length - read));
	uint64_t eocd_offset = (length - read);
	if (res < (int64_t)sizeof(structs::EndCentralDirectory)) {
		return;
	}
	// Scan back through the buffer looking for the header.
	for (int64_t i = res - sizeof(structs::EndCentralDirectory); i > 0; i--) {
		endCD = (structs::EndCentralDirectory*) (buffer.get() + i);
		if (le32toh(endCD->magic) == 0x6054b50) {
			eocd_offset += i;
			break;
		}
	}
	// Not found.
	if (le32toh(endCD->magic) != 0x6054b50) {
#if DEBUG
		fprintf( aff4::getDebugOutput(), "%s[%d] : Zip EOCD Not Found: %s\n", __FILE__, __LINE__, filename.c_str());
#endif
		return;
	}

	// Get the zip comment field.
	if (le16toh(endCD->comment_len) > 0) {
		std::unique_ptr<char[]> commentBuffer(new char[le16toh(endCD->comment_len)]);
		if (commentBuffer == nullptr) {
			return;
		}
		res = fileRead(commentBuffer.get(), le16toh(endCD->comment_len),
				eocd_offset + sizeof(structs::EndCentralDirectory));
		if (res > 0) {
			// Comment MAY have NUL termination, so ensure we don't grab it as well...
			size_t l = strlen(commentBuffer.get());
			l = (l < (size_t)res) ? l : (size_t)res;
			std::string rComment(commentBuffer.get(), l);
			
			this->comment = rComment;
#if DEBUG
			fprintf( aff4::getDebugOutput(), "%s[%d] : Zip Comment: %s\n", __FILE__, __LINE__, rComment.c_str());
#endif
		}
	}

	// Start reading the Central Directory constructing ZipEntry elements.
	uint64_t directoryOffset = le32toh(endCD->offset_of_cd);
	uint64_t directoryNumberOfEntries = le16toh(endCD->total_entries_in_cd);
	int64_t globalOffset = 0;
	// Traditional zip file - non 64 bit.
	if (((uint32_t)directoryOffset) != 0xffffffff) {
		globalOffset = (eocd_offset - le32toh(endCD->size_of_cd) - directoryOffset);
	} else {
		// This is a 64 bit archive, find the Zip64EndCD.

		structs::Zip64CDLocator locator;
		uint32_t magic = locator.magic;
		uint64_t locator_real_offset = eocd_offset - sizeof(structs::Zip64CDLocator);
		res = fileRead(&locator, sizeof(structs::Zip64CDLocator), locator_real_offset);

		if (res != sizeof(structs::Zip64CDLocator) || le32toh(locator.magic) != magic || le32toh(locator.disk_with_cd) != 0
				|| le32toh(locator.number_of_disks) != 1) {
			return;
		}

		// Although it may appear that we can use the Zip64CDLocator to locate the
		// Zip64EndCD record via it's offset_of_cd record this is not quite so. If
		// the zip file was appended to another file, the offset_of_cd field will
		// not be valid, as it still points to the old offset. In this case we also
		// need to know the global shift.

		structs::Zip64EndCD end_cd;
		magic = end_cd.magic;
		res = fileRead(&end_cd, sizeof(structs::Zip64EndCD), locator_real_offset - sizeof(structs::Zip64EndCD));

		if (res != sizeof(structs::Zip64EndCD) || le32toh(end_cd.magic) != magic) {
			return;
		}

		directoryOffset = le64toh(end_cd.offset_of_cd);
		directoryNumberOfEntries = le64toh(end_cd.number_of_entries_in_volume);

		// The global offset is now known:
		globalOffset = (locator_real_offset - sizeof(structs::Zip64EndCD) - le64toh(end_cd.size_of_cd) - directoryOffset);
	}

	// Now iterate over all entries in the CD...
	uint64_t entryOffset = directoryOffset;
#if DEBUG
	fprintf( aff4::getDebugOutput(), "%s[%d] : Zip directoryNumberOfEntries: %" PRIu64 "\n", __FILE__, __LINE__, directoryNumberOfEntries);
#endif
	for (uint64_t i = 0; i < directoryNumberOfEntries; i++) {
		structs::Zip64Entry zipEntry;
		structs::CDFileHeader entry;
		uint32_t magic = entry.magic;
		res = fileRead(&entry, sizeof(entry), globalOffset + entryOffset);
		if (res != sizeof(entry) || le32toh(entry.magic) != magic) {
			return;
		}
		zipEntry.size = le32toh(entry.file_size);
		zipEntry.csize = le32toh(entry.compress_size);
		zipEntry.headerOffset = le32toh(entry.relative_offset_local_header);

		// Read the segment filename.
		int64_t fieldOffset = globalOffset + entryOffset + sizeof(entry);
		std::unique_ptr<char[]> segmentNameBuffer(new char[le16toh(entry.file_name_length)]);
		if (segmentNameBuffer == nullptr) {
			return;
		}
		fileRead(segmentNameBuffer.get(), le16toh(entry.file_name_length), fieldOffset);
		std::string segmentName(segmentNameBuffer.get(), (size_t) le16toh(entry.file_name_length));
		fieldOffset += le16toh(entry.file_name_length);

		if (zipEntry.headerOffset == 0xffffffff) {
			// Zip64 entry...
			structs::ZipExtraFieldHeader extraHeader;
			uint16_t hOffset = 0;
			while (hOffset < le16toh(entry.extra_field_len)) {
				res = fileRead(&extraHeader, sizeof(extraHeader), fieldOffset + hOffset);
				if (res != sizeof(extraHeader)) {
					return;
				}
				if (le16toh(extraHeader.header_id) == 1) {
					uint16_t dataSize = le16toh(extraHeader.data_size);
					if (zipEntry.size == 0xffffffff && dataSize >= 8) {
						fileRead(&zipEntry.size, 8, fieldOffset + hOffset + sizeof(extraHeader));
						zipEntry.size = le64toh(zipEntry.size);
					}
					if (zipEntry.csize == 0xffffffff && dataSize >= 8) {
						fileRead(&zipEntry.csize, 8, fieldOffset + hOffset + sizeof(extraHeader));
						zipEntry.csize = le64toh(zipEntry.csize);
					}
					if (zipEntry.headerOffset == 0xffffffff && dataSize >= 8) {
						fileRead(&zipEntry.headerOffset, 8, fieldOffset + hOffset + sizeof(extraHeader));
						zipEntry.headerOffset = le64toh(zipEntry.headerOffset);
					}
				}
				hOffset += le16toh(extraHeader.data_size) + sizeof(extraHeader);
			}
		}
		// We now need to read in the actual segment header.
		zipEntry.dataOffset = zipEntry.headerOffset;
		structs::ZipFileHeader fileHeader;
		magic = fileHeader.magic;
		res = fileRead(&fileHeader, sizeof(fileHeader), zipEntry.dataOffset);
		if (res != sizeof(fileHeader) || le32toh(fileHeader.magic) != magic) {
			return;
		}
		zipEntry.dataOffset += sizeof(fileHeader) + le16toh(fileHeader.file_name_length) + le16toh(fileHeader.extra_field_len);

		// We should have all our information.
		std::shared_ptr<ZipEntry> segment(
				new ZipEntry(segmentName, zipEntry.headerOffset, zipEntry.dataOffset, zipEntry.size, zipEntry.csize,
						le16toh(entry.compression_method)));

		entries.push_back(segment);

		// Go to the next entry.
		entryOffset += (sizeof(entry) + le16toh(entry.file_name_length)
				+ le16toh(entry.extra_field_len) + le16toh(entry.file_comment_length));
	}
	errno = 0;
	return;
}

int64_t Zip::fileRead(void *buf, uint64_t count, uint64_t offset) noexcept {
	if ((count == 0) || (buf == nullptr)) {
		return 0;
	}
	// If offset beyond end, return.
	if (offset > length) {
		return 0;
	}
	// If offset + count, will go beyond end, truncate count.
	if (offset + count > length) {
		count -= (offset + count) - length;
	}
#ifndef _WIN32
	/*
	* POSIX based systems.
	*/
	return ::pread64(fileHandle, buf, count, offset);

#else 
	/*
	* Windows based systems
	*/
	// If our read size is greater than a DWORD, truncate the read.
	if (count > MAXDWORD) {
		count = MAXDWORD;
	}
	DWORD byteRead = (DWORD)count;
	DWORD bytesRead = 0;
	OVERLAPPED readDetails;
	readDetails.hEvent = NULL;
	readDetails.Internal = NULL;
	readDetails.InternalHigh = NULL;
	readDetails.Offset = (DWORD)(offset & 0xffffffffL);
	readDetails.OffsetHigh = (DWORD)((offset & 0xffffffff00000000L) >> 32);

	if (!ReadFile(fileHandle, buf, byteRead, &bytesRead, &readDetails)) {
		return -1;
	}
	return byteRead;
#endif
	
}

} /* namespace zip */
} /* namespace aff4 */
