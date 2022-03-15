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

#include "stdafx.h"
#include "xwaysPlugin.h"
#include "Util.h"

#include <cerrno>
#include <map>
#include <memory>
#include <atomic>
#include <mutex>
#include <inttypes.h>
#include <random>
#include <string>

/**
* Next handle.
*/
static std::atomic<INT> nextHandle = 1;

/*
* Lock for init/work/done operations.
*/
static std::mutex apiLock;

/**
* Typedef for held container references.
* <p>
* We have a resolver, the parent container, and the aff4:Stream for the map.s
*/
typedef typename std::tuple<//
	std::shared_ptr<aff4::IAFF4Resolver>, //
	std::shared_ptr<aff4::IAFF4Container>, //
	std::shared_ptr<aff4::IAFF4Stream>> container_t;

/**
* Handle for UI.
*/
static HWND mainWnd;

/**
* Map of open containers.
*/
static std::shared_ptr<std::map<int, container_t>> handles = std::make_shared<std::map<int, container_t>>();

/**
 * Debug logging options
 */
#if DEBUG_VERBOSE
FILE* loggerHandle = nullptr;

std::wstring randomString(std::wstring::size_type length)
{
	static auto& chrs = "0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	thread_local static std::mt19937 rg{ std::random_device{}() };
	thread_local static std::uniform_int_distribution<std::wstring::size_type> pick(0, sizeof(chrs) - 2);

	std::wstring s;

	s.reserve(length);

	while (length--)
		s += chrs[pick(rg)];

	return s;
}

static void setupLog() {
	if (loggerHandle != nullptr) {
		return;
	}
	// Create the file to log to, and set aff4::setDebugOutput()
	TCHAR lpTempPathBuffer[MAX_PATH];
	DWORD dwRetVal = 0;

	dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer); // buffer for path 
	if (dwRetVal > MAX_PATH || (dwRetVal == 0)) {
		return;
	}
	std::wstring filename(lpTempPathBuffer);
	filename = filename + L"\\libaff4."+ randomString(8) + L".log";
	loggerHandle = (FILE*)malloc(sizeof(FILE));
	ZeroMemory(loggerHandle, sizeof(FILE));
	errno_t res = _wfopen_s(&loggerHandle, filename.c_str(), L"w+");
	if (res == 0) {
		aff4::setDebugOutput(loggerHandle);
	} else {
		::free(loggerHandle);
		loggerHandle = nullptr;
	}
}

static void closeLog() {
	if (loggerHandle == nullptr) {
		return;
	}
#if DEBUG_VERBOSE
	fprintf(aff4::getDebugOutput(), "%s[%d] : Close Log File \n", __FILE__, __LINE__);
#endif
	aff4::setDebugOutput(stderr);
	fclose(loggerHandle);
	//::free(loggerHandle);
	loggerHandle = nullptr;
}

#endif

extern "C" {

	/*
	 * IIO_Init
	 */

	PVOID XWAYSAFF4_CALLING IIO_Init(
		HANDLE hMainWnd,
		LPWSTR lpFilePath,
		PVOID pHeaderBuf,
		DWORD nHeaderBufSize,
		struct ImageInfo* pImgInfo,
		PVOID pReserved
	) {

		std::lock_guard<std::mutex> lock(apiLock);
		mainWnd = (HWND)hMainWnd;

#if DEBUG_VERBOSE
		setupLog();
#endif

		if (lpFilePath == NULL) {
#if DEBUG_VERBOSE
			fprintf(aff4::getDebugOutput(), "%s[%d] : No Filename provided? \n", __FILE__, __LINE__);
#endif
			return NULL;
		}
		if (pImgInfo == NULL) {
#if DEBUG_VERBOSE
			fprintf(aff4::getDebugOutput(), "%s[%d] : No Image Information Packet provided? \n", __FILE__, __LINE__);
#endif
			return NULL;
		}

		/*
		* Set initial values.
		*/
		pImgInfo->lpTextualDescr = NULL;
		pImgInfo->nFlags = 0;
		pImgInfo->nSectorCount = 0;
		pImgInfo->nSectorSize = 512;
		pImgInfo->nSize = sizeof(ImageInfo);


		std::wstring wfile(lpFilePath);
		if (wfile.empty()) {
			errno = ENOENT;
#if DEBUG_VERBOSE
			fprintf(aff4::getDebugOutput(), "%s[%d] : Filename provided is empty? \n", __FILE__, __LINE__);
#endif
			return NULL;
		}
		// Convert wstring (utf-16) to string (utf-8)
		std::string file = ws2s(wfile);
#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : Attempt to open: %s \n", __FILE__, __LINE__, file.c_str());
#endif
		// Attempt to open the file.
		std::shared_ptr<aff4::IAFF4Container> container = aff4::container::openAFF4Container(file);
		if (container == nullptr) {
#if DEBUG_VERBOSE
			fprintf(aff4::getDebugOutput(), "%s[%d] : No AFF4 Container materialised? %s \n", __FILE__, __LINE__, file.c_str());
#endif
			if (!aff4::container::isAFF4Container(file)) {
#if DEBUG_VERBOSE
				fprintf(aff4::getDebugOutput(), "%s[%d] : Filename does not appear to be an AFF4 Container? %s \n", __FILE__, __LINE__, file.c_str());
#endif
				errno = ENOENT;
				return NULL;
			}
			std::string resource = aff4::container::getResourceID(file);
			pImgInfo->nFlags = IIO_INIT_ERROR_GIVE_UP;
			if (resource.empty()) {
#if DEBUG_VERBOSE
				fprintf(aff4::getDebugOutput(), "%s[%d] : Invalid AFF4 Container - Missing Resource Identifier? %s \n", __FILE__, __LINE__, file.c_str());
#endif
				pImgInfo->lpTextualDescr = createText(L"Invalid AFF4 Container - Missing Resource Identifier");
			} else {
				// Have a Resource ID? Possible empty container.
#if DEBUG_VERBOSE
				fprintf(aff4::getDebugOutput(), "%s[%d] : Invalid AFF4 Container - Missing Contents? %s \n", __FILE__, __LINE__, file.c_str());
#endif
				pImgInfo->lpTextualDescr = createText(L"Invalid AFF4 Container - Missing Contents");
			}
			// We MUST return a handle for the user to be given the above error message.
			return (PVOID)(nextHandle++);
		}

		// Attempt to start a resolver
		std::shared_ptr<aff4::IAFF4Resolver> resolver(aff4::container::createResolver(file, false));
		if (resolver == nullptr) {
#if DEBUG_VERBOSE
			fprintf(aff4::getDebugOutput(), "%s[%d] : Unable to construct Resolver at location. \n", __FILE__, __LINE__);
#endif
			errno = ENOENT;
			pImgInfo->nFlags = IIO_INIT_ERROR_GIVE_UP;
			pImgInfo->lpTextualDescr = createText(L"Unable to construct Resolver at location.");
			return (PVOID)(nextHandle++);
		}
		container->setResolver(resolver.get());

		// Open the first image.
		std::vector<std::shared_ptr<aff4::IAFF4Image>> images = container->getImages();
		if (images.empty()) {
#if DEBUG_VERBOSE
			fprintf(aff4::getDebugOutput(), "%s[%d] : No Images contained or available in the AFF4 Container.\n", __FILE__, __LINE__);
#endif
			errno = ENOENT;
			pImgInfo->nFlags = IIO_INIT_ERROR_GIVE_UP;
			pImgInfo->lpTextualDescr = createText(L"No Images contained or available in the AFF4 Container.");
			return (PVOID)(nextHandle++);
		}
		if (images[0] == nullptr) {
#if DEBUG_VERBOSE
			fprintf(aff4::getDebugOutput(), "%s[%d] : No Images contained or available in the AFF4 Container.\n", __FILE__, __LINE__);
#endif
			errno = ENOENT;
			pImgInfo->nFlags = IIO_INIT_ERROR_GIVE_UP;
			pImgInfo->lpTextualDescr = createText(L"No Images contained or available in the AFF4 Container.");
			return (PVOID)(nextHandle++);
		}
		// Get the map for the first image.
		std::shared_ptr<aff4::IAFF4Map> map = images[0]->getMap();
		if (map == nullptr) {
#if DEBUG_VERBOSE
			fprintf(aff4::getDebugOutput(), "%s[%d] : Primary Image is missing required Datastream Map.\n", __FILE__, __LINE__);
#endif
			errno = ENOENT;
			pImgInfo->nFlags = IIO_INIT_ERROR_GIVE_UP;
			pImgInfo->lpTextualDescr = createText(L"Primary Image is missing required Datastream Map.");
			return (PVOID)(nextHandle++);
		}
		std::shared_ptr<aff4::IAFF4Stream> stream = map->getStream();
		if (stream == nullptr) {
#if DEBUG_VERBOSE
			fprintf(aff4::getDebugOutput(), "%s[%d] : Datastream for Image is not available or corrupted.\n", __FILE__, __LINE__);
#endif
			errno = ENOENT;
			pImgInfo->nFlags = IIO_INIT_ERROR_GIVE_UP;
			pImgInfo->lpTextualDescr = createText(L"Datastream for Image is not available or corrupted.");
			return (PVOID)(nextHandle++);
		}
		container_t handleEntry = std::make_tuple(resolver, container, stream);
		int handle = nextHandle++;
		(*handles)[handle] = handleEntry;
#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : Created aff4:Image instance. Handle %d\n", __FILE__, __LINE__, handle);
#endif
		/*
		* Add return parameters.
		*/
		pImgInfo->nFlags = IIO_INIT_READ | IIO_INIT_THREADSAFE | IIO_INIT_UNALIGNED_OK;

		// Determine Disk or Volume.
		std::shared_ptr<aff4::IAFF4Image> image = images[0];
		std::vector<aff4::rdf::RDFValue> properties = image->getProperty(aff4::Lexicon::AFF4_TYPE);
		if (!properties.empty()) {
			for (aff4::rdf::RDFValue v : properties) {
				if (v.getType() == aff4::Lexicon::AFF4_DISK_IMAGE_TYPE) {
#if DEBUG_VERBOSE
					fprintf(aff4::getDebugOutput(), "%s[%d] : Setting Disk Image\n", __FILE__, __LINE__);
#endif
					pImgInfo->nFlags |= IIO_INIT_DISK;
					break;
				}
				if (v.getType() == aff4::Lexicon::AFF4_VOLUME_IMAGE_TYPE) {
#if DEBUG_VERBOSE
					fprintf(aff4::getDebugOutput(), "%s[%d] : Setting Volume Image\n", __FILE__, __LINE__);
#endif
					pImgInfo->nFlags |= IIO_INIT_VOLUME;
					break;
				}
			}
		}
		if ((pImgInfo->nFlags & (IIO_INIT_DISK | IIO_INIT_VOLUME)) == 0) {
			// Disk/Volume type not set, check for Black Bag Technologies container type property
			properties = image->getProperty(aff4::Lexicon::BBT_APFS_CONTAINER_TYPE);
			if (!properties.empty()) {
				for (aff4::rdf::RDFValue v : properties) {
					if (v.getType() == aff4::Lexicon::BBT_APFS_CONTAINER_TYPE_T2
						|| v.getType() == aff4::Lexicon::BBT_APFS_CONTAINER_TYPE_FUSION
						|| v.getType() == aff4::Lexicon::BBT_APFS_CONTAINER_TYPE_STANDARD) {
						// Big assumption that these all represent a volume... 
						// I'm sure someone will file a bug report if this is incorrect.
#if DEBUG_VERBOSE
						fprintf(aff4::getDebugOutput(), "%s[%d] : Setting Volume Image (bbt:Std/T2/Fusion)\n", __FILE__, __LINE__);
#endif
						pImgInfo->nFlags |= IIO_INIT_VOLUME;
						break;
					}
				}
			}
		}

		// Get Sector Size.
		properties = image->getProperty(aff4::Lexicon::AFF4_BLOCKSIZE);
		if (!properties.empty()) {
			aff4::rdf::RDFValue v = properties[0];
			if (v.getXSDType() == aff4::rdf::XSDType::Int) {
				pImgInfo->nSectorSize = (DWORD)v.getInteger();
			}
			else if (v.getXSDType() == aff4::rdf::XSDType::Long) {
				pImgInfo->nSectorSize = (DWORD)v.getLong();
			}
		}
#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : Setting Sector Size %d\n", __FILE__, __LINE__, pImgInfo->nSectorSize);
#endif

		// Get Sector Count.
		properties = image->getProperty(aff4::Lexicon::AFF4_DISK_DEVICE_SECTOR_COUNT);
		if (!properties.empty()) {
			aff4::rdf::RDFValue v = properties[0];
			if (v.getXSDType() == aff4::rdf::XSDType::Int) {
				pImgInfo->nSectorCount = (INT64)v.getInteger();
			}
			else if (v.getXSDType() == aff4::rdf::XSDType::Long) {
				pImgInfo->nSectorCount = v.getLong();
			}
		}
		else {
			// No defined sector size, so get the size of the stream and divide by block size.
			pImgInfo->nSectorCount = (INT64)stream->size() / (INT64)pImgInfo->nSectorSize;
		}
#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : Setting Sector Count %" PRIi64 "\n", __FILE__, __LINE__, pImgInfo->nSectorCount);
#endif
		/*
		* Now: what do we do about returning metadata?
		*/
		pImgInfo->lpTextualDescr = getMetaData(image);
		return (PVOID)handle;
	};

	/*
	* IIO_Work
	*/

	INT64 XWAYSAFF4_CALLING IIO_Work(
		PVOID lpImage,
		INT64 nOfs,
		INT64 nSize,
		PVOID lpBuffer,
		PBYTE pFlags
	) {
		// Check basic parameters.
		if ((lpImage == NULL) || (lpBuffer == NULL) || (nOfs < 0) || (nSize <= 0)) {
			return 0;
		}

		// Allow pFlags to be NULL, then assume no flags.
		BYTE flags = (pFlags != NULL) ? *pFlags : 0;
		if ((flags & IIO_WRITE) == IIO_WRITE) {
			return 0;
		}

		container_t container;
		// Find our image.
		INT handle = (INT)lpImage;
		try {
			std::lock_guard<std::mutex> lock(apiLock);
			auto it = handles->find(handle);
			if (it != handles->end()) {
				container = it->second;
			}
			else {
#if DEBUG_VERBOSE
				fprintf(aff4::getDebugOutput(), "%s[%d] : Unknown Handle %d\n", __FILE__, __LINE__, handle);
#endif
				errno = ENOENT;
				return 0;
			}
		}
		catch (...) {
			errno = ENOENT;
			return 0;
		}
		std::shared_ptr<aff4::IAFF4Stream> stream = std::get<2>(container);

#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : (%d) Read : %" PRIx64 ":%" PRIx64 "\n", __FILE__, __LINE__, handle, nOfs, nSize);
#endif

		// See if a request for Sparse detection was requested.
		if ((flags & IIO_CHECK_FOR_SPARSE) == IIO_CHECK_FOR_SPARSE) {
			// Ensure to cap at end of disk
			INT64 length = stream->size();
			// If offset + size, will go beyond end, truncate size.
			if ((nOfs + nSize) > length) {
				nSize -= ((nOfs + nSize) - length);
				if (nSize <= 0) {
					// Do touch the bits...
/*					if (pFlags != NULL) {
						// Set not sparse.
						*pFlags = 0;
					}*/
					return 0;
				}
			}
			if (nSize > 0) {
				if (isSparse(stream, nOfs, nSize)) {
					// Only set the bit as required leaving the contents as is.
					*pFlags |= IIO_SPARSE_DETECTED;
#if DEBUG_VERBOSE
					fprintf(aff4::getDebugOutput(), "%s[%d] : (%d) Returning Sparse Region %" PRIx64 ":%" PRIx64 "\n", __FILE__, __LINE__, handle, nOfs, nSize);
#endif
					return nSize;
				}
				// We are not sparse, so fall through.
			}
		}

		// Else fall through can complete the read as per normal.
		/*if (pFlags != NULL) {
			// Set not sparse.
			*pFlags = 0;
		}*/
		int64_t read = stream->read(lpBuffer, nSize, nOfs);
		if (read < 0) {
			read = 0;
		}
#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : (%d) Read complete: %" PRIx64 ":%" PRIx64 " => %" PRIx64 "\n", __FILE__, __LINE__, handle, nOfs, nSize, read);
#endif
		return read;
	};

	/*
	* IIO_Done
	*/

	DWORD XWAYSAFF4_CALLING IIO_Done(
		PVOID lpImage,
		LPWSTR lpTextualDescr
	) {
		std::lock_guard<std::mutex> lock(apiLock);
		INT handle = (INT)lpImage;
#if DEBUG_VERBOSE
		fprintf(aff4::getDebugOutput(), "%s[%d] : Close Handle %d\n", __FILE__, __LINE__, handle);
#endif
		auto it = handles->find(handle);
		if (it != handles->end()) {
			container_t con = it->second;
			// Close the container
			std::get<1>(con)->close();
			// And remove the map entry.
			handles->erase(it);
		}
#if DEBUG_VERBOSE
		else {
			fprintf(aff4::getDebugOutput(), "%s[%d] : Unknown Handle %d\n", __FILE__, __LINE__, handle);
		}
#endif
		if (lpTextualDescr != NULL) {
			::free(lpTextualDescr);
		}

#if DEBUG_VERBOSE
		if (handles->empty()) {
			closeLog();
		}
#endif
		return 1;
	};

}

