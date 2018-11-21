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

#pragma once

#include "stdafx.h"

#include "aff4config.h"
#include "aff4.h"

// return this to signal read support for that image type
#define IIO_INIT_READ 0x0001 
// return this to signal write support for that image type
#define IIO_INIT_WRITE 0x0002
// image is known to represent a physical storage device, possibly partitioned
#define IIO_INIT_DISK 0x0010 
// image is known to represent a volume/partition
#define IIO_INIT_VOLUME 0x0020 
// your functions are thread-safe (currently a must)
#define IIO_INIT_THREADSAFE 0x0100 
// your functions support sector-unaligned I/O requests (not a must)
#define IIO_INIT_UNALIGNED_OK 0x0200 
// errors in the image have been detected, but it can presumable be read completely
#define IIO_INIT_ERROR_MILD 0x1000 
// errors in the image have been detected, it cannot be fully read
#define IIO_INIT_ERROR_SEVERE 0x2000 
// image type generally supported, but unable to open this particular file, lpTextualDescr points to an error message
#define IIO_INIT_ERROR_GIVE_UP 0x4000 

// in, write instead of read
#define IIO_WRITE 0x01 
// in, callee should check for and signal sparse areas if possible
#define IIO_CHECK_FOR_SPARSE 0x20
// out, requested range of data is entirely sparse
#define IIO_SPARSE_DETECTED 0x40 

#define XWAYSAFF4_API __declspec(dllexport)

#ifdef _WIN64 
#define XWAYSAFF4_CALLING 
#else
#define XWAYSAFF4_CALLING __stdcall
// X-Ways uses stdcall convention, but requires demangled names.
#pragma comment(linker, "/EXPORT:IIO_Init=_IIO_Init@24")
#pragma comment(linker, "/EXPORT:IIO_Work=_IIO_Work@28")
#pragma comment(linker, "/EXPORT:IIO_Done=_IIO_Done@8")
#endif

extern "C" {

#pragma pack(2) 
	struct ImageInfo {
		DWORD nSize;
		INT64 nSectorCount;
		DWORD nSectorSize;
		DWORD nFlags;
		LPWSTR lpTextualDescr;
	};

	/*
	Will be called when X-Ways Forensics is about to interpret a presumed image 
	file. You are given a pointer to a null-terminated file path in UTF-16 Unicode 
	(lpFilePath) and a pointer to a buffer (lpFilePath) with the first few 
	(nHeaderBufSize) bytes of the file contents. You can deduct directly from 
	the filename or filename extension  and/or those few bytes or indirectly 
	(by reading more from the file contents or looking for other files in the same 
	directory) whether your DLL supports this kind of image. If so, you return a 
	pointer to a private structure of yours, by which you identify that image in 
	future function calls, plus you have to fill the structure pointed to by 
	pImgInfo to give the caller some information about the image so that it can 
	be treated correctly. If you do not support the specified file type, you simply 
	return NULL.

	If you return a non-NULL pointer, but the caller is not happy with how you filled
	the ImageInfo structure (for example, unsupported value of nSize returned or flag 
	IIO_INIT_READ mysteriously not set), you can still expect a quick call of your 
	IIO_Done function so that you can at least tidy up after yourself, but you will 
	not be actually called for any I/O work for that image. The ImageInfo structure 
	is initially guaranteed to be zeroed except for the nSize part.

	If you open a file handle to the image file, you have to choose a sharing mode 
	for that, not an exclusive mode.

	hMainWnd: Handle of the main application window in case you need it, for example 
	as a parent window for a message box or dialog box that asks the user for 
	preferences or credentials etc.

	nSize: The size of the ImageInfo structure. This (and only this) member variable 
	is already set by the caller, and its value is currently 4+8+4+4+8=28 bytes in x64 
	(4+8+4+4+4=24 in x86), but it may be larger in future versions of X-Ways Forensics. 
	You do not currently have to check the value (only if you are curious), but you
	have to overwrite it with the size that you support. Should a future version of 
	X-Ways Forensics work with a larger structure, the original part remains the same 
	and newly added member variables are appended, but the caller will not expect 
	you to have filled them if the nSize that you set is still the smaller size of 
	older versions of this API.

	nSectorCount: The number of sectors represented by the image.

	nSectorSize: Size of one sector in bytes. Currently supported sector sizes in X-Way 
	Forensics are 512, 1024, 2048, 4096, and 8192 bytes.

	nFlags: A combination of the below flags.
	#define IIO_INIT_READ 0x0001 // return this to signal read support for that image type
	#define IIO_INIT_WRITE 0x0002 // return this to signal write support for that image type
	#define IIO_INIT_DISK 0x0010 // image is known to represent a physical storage device, possibly partitioned
	#define IIO_INIT_VOLUME 0x0020 // image is known to represent a volume/partition
	#define IIO_INIT_THREADSAFE 0x0100 // your functions are thread-safe (currently a must)
	#define IIO_INIT_UNALIGNED_OK 0x0200 // your functions support sector-unaligned I/O requests (not a must)
	#define IIO_INIT_ERROR_MILD 0x1000 // errors in the image have been detected, but it can presumable be read completely
	#define IIO_INIT_ERROR_SEVERE 0x2000 // errors in the image have been detected, it cannot be fully read
	#define IIO_INIT_ERROR_GIVE_UP 0x4000 // image type generally supported, but unable to open this particular file, lpTextualDescr points to an error message

	It is OK to set neither IIO_INIT_DISK not IIO_INIT_VOLUME if you do not know 
	what kind of image it is. In that case X-Ways Forensics will make its own 
	determination or might prompt the user.

	lpTextualDescr: Optional. A null-terminated string that contains textual metadata 
	about the image, such as who created it, how and when, using which tool, with 
	which compression option, etc. You can allocate that buffer in memory with any 
	compiler-specific memory allocation function or Windows API function of your 
	choice. You will be asked to release it when your IIO_Done() function is called.
	*/
	XWAYSAFF4_API PVOID XWAYSAFF4_CALLING IIO_Init(
		HANDLE hMainWnd,
		LPWSTR lpFilePath,
		PVOID pHeaderBuf,
		DWORD nHeaderBufSize,
		struct ImageInfo* pImgInfo,
		PVOID pReserved
	);

	/*
	This function is called to read nSize bytes from offset nOfs in the image identified 
	by your own pointer lpImage, i.e. the result of IIO_Init(), into the memory buffer 
	pointer to by lpBuffer. It might also be called to write that many bytes at the 
	specified offset, provided that you have signaled write capability in IIO_Init().

	pFlags: Some flags are set by the caller, some additional flags may be set by the 
	callee. In a simple implementation, if you did not signal write capability and do 
	not provide sparse support, you can simply ignore this flags field altogether.
	Do NOT alter the current contents of the flags, only set the required bits if applicable.

	#define IIO_WRITE 0x01 // in, write instead of read
	#define IIO_CHECK_FOR_SPARSE 0x20 // in, callee should check for and signal sparse areas if possible
	#define IIO_SPARSE_DETECTED 0x40 // out, requested range of data is entirely sparse

	Please return the number of successfully read/written bytes. IIO_SPARSE_DETECTED should 
	not be returned if IIO_CHECK_FOR_SPARSE was not set. If IIO_CHECK_FOR_SPARSE was set 
	and you return IIO_CHECK_FOR_SPARSE, you don't have to fill the buffer at all, but 
	you are still supposed to return the total number of bytes covered/requested. The 
	caller takes the IIO_CHECK_FOR_SPARSE flag as a hint that the data in the requested 
	range is ignorable and will save time by not processing it.
	*/
	XWAYSAFF4_API INT64 XWAYSAFF4_CALLING IIO_Work(
		PVOID lpImage,
		INT64 nOfs,
		INT64 nSize,
		PVOID lpBuffer,
		PBYTE pFlags
	);

	/*
	When the image is closed/no further I/O is required, X-Ways Forensics calls this 
	function to give you a chance to free up any resources such as file handles, that 
	you were keeping open since the Init function was called. You are given the pointer
	that your IIO_Init() function had returned, and you are also asked to release the 
	buffer for the textual description if you had provided one. When done, please 
	return 1.
	*/
	XWAYSAFF4_API DWORD XWAYSAFF4_CALLING IIO_Done(
		PVOID lpImage,
		LPWSTR lpTextualDescr
	);

};
