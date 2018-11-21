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

#include <algorithm>

#ifndef _WIN32
#include <libgen.h>
#endif

#ifdef _WIN32
#include <Shlwapi.h>
#endif

#include "AFF4Containers.h"
#include "Zip.h"
#include "AFF4ZipContainer.h"
#include "StringUtil.h"
#include "FileUtil.h"
#include "LightResolver.h"
#include "PortableEndian.h"

#define AFF4_RESOURCE_BUFFER_SIZE 4096

namespace aff4 {
	namespace container {

		/**
		 * Open the given file as an AFF4 Container
		 *
		 * @param filename The file to open
		 * @return A AFF4 container instance
		 * @throws std::system_error If opening the file failed.
		 */
		std::shared_ptr<IAFF4Container> openContainer(const std::string& filename) noexcept {
#if DEBUG
			fprintf(aff4::getDebugOutput(), "%s[%d] : %s \n", __FILE__, __LINE__, filename.c_str());
#endif
			/*
			 * Does it exist and is a file?
			 */
			if (!aff4::util::isFile(filename)) {
				// failed.
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : %s is NOT a file.\n", __FILE__, __LINE__, filename.c_str());
#endif
				return nullptr;
			}

			/*
			 * Check filename
			 */
			if (!isAFF4Container(filename)) {
				// failed.
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : %s does not have .af4 or .aff4 extension\n", __FILE__, __LINE__, filename.c_str());
#endif
				return nullptr;
			}

			std::string resource = getResourceID(filename);
			if (resource.empty()) {
				// No resource?
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : %s does not have a resource ID\n", __FILE__, __LINE__, filename.c_str());
#endif
				return nullptr;
			}
			/*
			 * Attempt to load as Zip container. (Valid containers will have at least 2 entries).
			 */
			std::unique_ptr<aff4::zip::Zip> zipFile(new aff4::zip::Zip(filename));
			if (zipFile->getEntries().empty()) {
				/*
				 * failed, actually, we should never see this, unless the file has been removed between the last
				 * getResourceID() call and reopening it here.
				 */
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : %s has NO entries?\n", __FILE__, __LINE__, filename.c_str());
#endif
				return nullptr;
			}
			// Construct the container.
			std::shared_ptr<AFF4ZipContainer> container = std::make_shared<AFF4ZipContainer>(resource, std::move(zipFile));
			return container;
		}

		bool isAFF4Container(std::string filename) noexcept {
			// Cheap nasty not really unicode transformation to lower case.
			std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
			return aff4::util::hasSuffix(filename, ".af4") || aff4::util::hasSuffix(filename, ".aff4");
		}

		std::string getResourceID(const std::string& filename) noexcept {
			/*
			 * Evimetry will store container.description as the very first ZIP file entry.
			 * Whilst not strictly part of the AFF4 Specification to do so, this is the normal
			 * expected implementation behaviour.
			 *
			 * Knowing, this allows to just read the very first entry and parse it for the container UUID.
			 * (Much quicker than a full Zip container open/scan/etc).
			 *
			 * To use the slow full open zip container method define the definition as per below.
			 */
#ifndef AFF4_USE_FULL_CONTAINER_OPEN_FOR_RESOURCE

			std::unique_ptr<uint8_t[]> buffer(new uint8_t[AFF4_RESOURCE_BUFFER_SIZE]);
			/*
			* Load in the first 4K bytes of the file
			*/
#ifndef _WIN32
			/*
			* POSIX based systems.
			*/
			int fileHandle = ::open(filename.c_str(), O_RDONLY | O_LARGEFILE);
			if (fileHandle == -1) {
				// we failed, so return nothing. (error will be in errno).
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : Unable to open container : %s \n", __FILE__, __LINE__, filename.c_str());
#endif
				return "";
			}
			int read = ::pread64(fileHandle, buffer.get(), AFF4_RESOURCE_BUFFER_SIZE, 0);
			::close(fileHandle);
			if (read != AFF4_RESOURCE_BUFFER_SIZE) {
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : Unable to read %d bytes from the container : %s = %d \n", __FILE__, __LINE__, AFF4_RESOURCE_BUFFER_SIZE, filename.c_str(), read);
#endif
				return "";
			}
#else
			/*
			* Windows based systems
			*/
			// Note: DO NOT ADD OVERLAPPED ATTRIBUTE for opening the file.
			std::wstring wpath = aff4::util::s2ws(filename); // Convert the filename to UTF-16/WString for Win32 API
			HANDLE fileHandle = CreateFile(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS, NULL);
			if (fileHandle == INVALID_HANDLE_VALUE) {
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : Unable to open container : %s \n", __FILE__, __LINE__, filename.c_str());
#endif
				return "";
			}
			DWORD read = 0;
			BOOL res = ReadFile(fileHandle, buffer.get(), AFF4_RESOURCE_BUFFER_SIZE, &read, NULL);
			::CloseHandle(fileHandle);
			if (res == FALSE || read != AFF4_RESOURCE_BUFFER_SIZE) {
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : Unable to read %d bytes from the container : %s = %d \n", __FILE__, __LINE__, AFF4_RESOURCE_BUFFER_SIZE, filename.c_str(), read);
#endif
				return "";
			}
#endif

			/*
			* The buffer should now contain the first 512bytes of the container.
			*/
			aff4::zip::structs::ZipFileHeader* header = (aff4::zip::structs::ZipFileHeader*)(buffer.get());
			// Start sanity check.
			if (header->magic != 0x4034b50) {
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : Invalid PKZIP Magic number : %s = 0x%x \n", __FILE__, __LINE__, filename.c_str(), header->magic);
#endif
				return "";
			}
			if (header->compression_method != 0) {
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : Invalid PKZIP compression method : %s = %d \n", __FILE__, __LINE__, filename.c_str(), header->compression_method);
#endif
				return "";
			}
			if (header->file_name_length != 0x15) { // 'container.description'
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : Invalid segment name length : %s = %d != 0x15\n", __FILE__, __LINE__, filename.c_str(), header->file_name_length);
#endif
				return "";
			}
			// Get the filename.
			size_t filenameLength = le16toh(header->file_name_length);
			std::string segmentName((char*)(buffer.get() + sizeof(aff4::zip::structs::ZipFileHeader)), filenameLength);
			if (segmentName.compare(AFF4_FILEDESCRIPTOR) != 0) {
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : Invalid segment name : %s = %s\n", __FILE__, __LINE__, filename.c_str(), segmentName.c_str());
#endif
				return "";
			}
			size_t segmentEntrySize = 0;
			// At this point the segment name checks out, so let get the content.
			if (header->file_size == -1) {
				// We need to scan for the ZipDataDescriptor64 structure... this should be in the first 512 bytes that we have read.	
				int dd64magic = 0x08074b50;
				size_t dd64Size = sizeof(aff4::zip::structs::ZipDataDescriptor64);
				uint8_t* offset = buffer.get();
				aff4::zip::structs::ZipDataDescriptor64* dd64 = (aff4::zip::structs::ZipDataDescriptor64*)offset;
				while (dd64->magic != dd64magic) {
					offset++;
					if (offset > (buffer.get() + AFF4_RESOURCE_BUFFER_SIZE - dd64Size)) {
						break;
					}
					dd64 = (aff4::zip::structs::ZipDataDescriptor64*)offset;
				}
				if (dd64->magic == dd64magic) {
					segmentEntrySize = dd64->compress_size;
				}
			} else {
				segmentEntrySize = header->file_size;
			}
			if ((segmentEntrySize == 0) || //
				(segmentEntrySize > (AFF4_RESOURCE_BUFFER_SIZE - (sizeof(aff4::zip::structs::ZipFileHeader) - header->file_name_length - header->extra_field_len)))) {
				// Too large...
#if DEBUG
				fprintf(aff4::getDebugOutput(), "%s[%d] : Segment appears too large for container.description? : %s \n", __FILE__, __LINE__, filename.c_str());
#endif
				return "";
			}
			// And get our data.
			char* data = (char*)buffer.get() + sizeof(aff4::zip::structs::ZipFileHeader) + header->file_name_length + header->extra_field_len;
			std::string resource(data, segmentEntrySize);
			return resource;
#else

			 /*
			  * Attempt to load as Zip container. (Valid containers will have at least 2 entries).
			  */
			std::unique_ptr<aff4::zip::Zip> zipFile(new aff4::zip::Zip(filename));
			if (zipFile->getEntries().empty()) {
				// failed.
				return "";
			}
			// Get the Zip comment.
			std::string resource = zipFile->getZipComment();
			// And now look for the container.description file. (it's contents overrides the zip comment field).
			std::string description(AFF4_FILEDESCRIPTOR);
			std::shared_ptr<IAFF4Stream> stream = zipFile->getStream(description);
			if (stream != nullptr) {
				std::unique_ptr<char[]> buffer(new char[stream->size()]);
				int64_t res = stream->read(buffer.get(), stream->size(), 0);
				if (res > 0) {
					resource = std::string(buffer.get(), res);
				}
				stream->close();
			}
			zipFile->close();
			return resource;
#endif
		}

		std::shared_ptr<IAFF4Container> openAFF4Container(const std::string& filename) noexcept {
			std::shared_ptr<IAFF4Container> container = openContainer(filename);
			if (container != nullptr) {
				container->setResolver(nullptr);
			}
			return container;
		}

		std::shared_ptr<IAFF4Container> openAFF4Container(const std::string& filename, IAFF4Resolver* resolver) noexcept {
			std::shared_ptr<IAFF4Container> container = openContainer(filename);
			if (container != nullptr) {
				container->setResolver(resolver);
			}
			return container;
		}

		aff4::IAFF4Resolver* createResolver(std::string path, bool scanSubFolders) noexcept {
			if (path.empty()) {
				return nullptr;
			}
			while (aff4::util::hasSuffix(path, "/")) {
				path = path.substr(0, path.length() - 1);
			}
			if (path.empty()) {
				return nullptr;
			}
			/*
			 * See if it exists.
			 */
			if (!aff4::util::fileExists(path)) {
				return nullptr;
			}

			if (aff4::util::isFile(path)) {
#ifdef _WIN32
				/*
				* Windows based systems
				*/
				// is a file.
				std::wstring filename = aff4::util::s2ws(path);

				std::unique_ptr<wchar_t[]> buffer(new wchar_t[filename.size() + 2]);
				std::memset(buffer.get(), 0, filename.size() * 2 + 2);
				::memcpy_s((char*)buffer.get(), (filename.size() * 2) + 1, (char*)filename.c_str(), (filename.size() * 2));

				PathRemoveFileSpec(buffer.get());
				std::wstring tmpPath(buffer.get());
				std::string basePath = aff4::util::ws2s(tmpPath);
# else 
				/*
				* POSIX based systems.
				*/
				// is a file.
				std::unique_ptr<char[]> buffer(new char[path.size() + 2]);
				std::memset(buffer.get(), 0, path.size() + 2);
				std::strncpy(buffer.get(), path.c_str(), path.size());
				// dirname may modify the contents, hence the copy.
				char* dir = ::dirname(buffer.get());
				std::string basePath(dir);
#endif
				return createResolver(basePath);
			}
			return new aff4::resolver::LightResolver(aff4::util::generateID(), path, scanSubFolders);
		}

	} /* namespace container */
} /* namespace aff4 */

