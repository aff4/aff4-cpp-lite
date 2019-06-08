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
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "xwaysPlugin.h"
#include <openssl/sha.h>
#include <shlwapi.h>

#define UNITTEST_BASE_PATH L"..\\..\\..\\"

namespace xwaysPluginUnitTest
{
	TEST_CLASS(UnitTestXwaysPlugin)
	{
	public:

		/**
		* Get the SHA1 sum of the image exported.
		*/
		std::string sha1sum(PVOID handle, INT64 size, INT64 readSize) {
			if (handle == nullptr) {
				return "Failed: no stream?";
			}

			unsigned char hash[SHA_DIGEST_LENGTH];
			char hashStr[(SHA_DIGEST_LENGTH * 2) + 2];

			SHA_CTX ctx;
			SHA1_Init(&ctx);

			INT64 toRead = size;
			INT64 offset = 0;
			INT64 bufferSize = readSize;
			// Read in buffer fulls
			std::unique_ptr<uint8_t[]> buffer(new uint8_t[bufferSize]);
			if (buffer.get() == nullptr) {
				return "Failed buffer malloc()";
			}
			while (toRead > 0) {
				INT64 sz = (bufferSize < toRead) ? bufferSize : toRead;
				INT64 res = IIO_Work(handle, offset, sz, buffer.get(), NULL);

				if (res > 0) {
					SHA1_Update(&ctx, buffer.get(), res);
					toRead -= res;
				}
				else {
					// read failed?
					toRead = 0;
				}
				offset += res;
			}
			SHA1_Final(hash, &ctx);

			// Convert to string.
			for (int j = 0; j < SHA_DIGEST_LENGTH; j++) {
				::sprintf(&hashStr[2 * j], "%02x", hash[j]);
			}
			std::string result((char*)&hashStr, (SHA_DIGEST_LENGTH * 2));
			return result;
		}

		/**
		* Check the sparseness of the region.
		*/
		void checkIsSparse(PVOID handle, INT64 offset, INT64 size, DWORD flag) {
			std::unique_ptr<uint8_t[]> buffer(new uint8_t[size]);

			// NO sparse check
			BYTE flags = 0;
			INT64 res = IIO_Work(handle, offset, size, buffer.get(), &flags);
			Assert::AreEqual((UINT64)size, (UINT64)res);
			Assert::IsTrue((flags & IIO_SPARSE_DETECTED) == 0);

			// Sparse check 
			flags = IIO_CHECK_FOR_SPARSE;
			res = IIO_Work(handle, offset, size, buffer.get(), &flags);
			Assert::AreEqual((UINT64)size, (UINT64)res);
			Assert::IsTrue((flags & IIO_SPARSE_DETECTED) == flag);
		}

		TEST_METHOD(testInitNULLFilename)
		{
			ImageInfo info;
			info.nSize = sizeof(ImageInfo);
			info.nFlags = 0;
			info.nSectorCount = 0;
			info.nSectorSize = 0;
			info.lpTextualDescr = NULL;

			PVOID handle = IIO_Init(NULL, NULL, NULL, 0, &info, NULL);
			Assert::IsNull(handle);
		}

		TEST_METHOD(testInitNULLImgInfo)
		{
			std::wstring filename(UNITTEST_BASE_PATH "tests\\resources\\Base-Linear.aff4");

			PVOID handle = IIO_Init(NULL, (LPWSTR)filename.c_str(), NULL, 0, NULL, NULL);
			Assert::IsNull(handle);
		}

		TEST_METHOD(testBasicLinearOpen)
		{
			std::wstring filename(UNITTEST_BASE_PATH "tests\\resources\\Base-Linear.aff4");
			ImageInfo info;
			info.nSize = sizeof(ImageInfo);
			info.nFlags = 0;
			info.nSectorCount = 0;
			info.nSectorSize = 0;
			info.lpTextualDescr = NULL;

			PVOID handle = IIO_Init(NULL, (LPWSTR)filename.c_str(), NULL, 0, &info, NULL);
			Assert::IsNotNull(handle);

			Assert::AreEqual((UINT64)524288, (UINT64)info.nSectorCount);
			Assert::AreEqual((DWORD)512, info.nSectorSize);
			Assert::AreEqual((DWORD)IIO_INIT_READ | IIO_INIT_THREADSAFE | IIO_INIT_UNALIGNED_OK | IIO_INIT_DISK, info.nFlags);

			Assert::IsNotNull(info.lpTextualDescr);
			std::wstring textDesc(info.lpTextualDescr);

			IIO_Done(handle, info.lpTextualDescr);
		}

		TEST_METHOD(testBasicLinearRead)
		{
			std::wstring filename(UNITTEST_BASE_PATH "tests\\resources\\Base-Linear.aff4");
			ImageInfo info;
			info.nSize = sizeof(ImageInfo);
			info.nFlags = 0;
			info.nSectorCount = 0;
			info.nSectorSize = 0;
			info.lpTextualDescr = NULL;

			PVOID handle = IIO_Init(NULL, (LPWSTR)filename.c_str(), NULL, 0, &info, NULL);
			Assert::IsNotNull(handle);

			Assert::AreEqual((UINT64)524288, (UINT64)info.nSectorCount);
			Assert::AreEqual((DWORD)512, info.nSectorSize);
			Assert::AreEqual((DWORD)IIO_INIT_READ | IIO_INIT_THREADSAFE | IIO_INIT_UNALIGNED_OK | IIO_INIT_DISK, info.nFlags);

			Assert::IsNotNull(info.lpTextualDescr);
			std::wstring textDesc(info.lpTextualDescr);
			std::wcout << textDesc << std::endl;

			std::string sha1 = sha1sum(handle, info.nSectorCount * info.nSectorSize, 128 * 1024);
			std::string expectedSHA1 = "7d3d27f667f95f7ec5b9d32121622c0f4b60b48d";
			Assert::AreEqual(expectedSHA1, sha1);

			IIO_Done(handle, info.lpTextualDescr);
		}

		TEST_METHOD(testAllocatedSparseMap)
		{
			std::wstring filename(UNITTEST_BASE_PATH "tests\\resources\\Base-Allocated.aff4");
			ImageInfo info;
			info.nSize = sizeof(ImageInfo);
			info.nFlags = 0;
			info.nSectorCount = 0;
			info.nSectorSize = 0;
			info.lpTextualDescr = NULL;

			PVOID handle = IIO_Init(NULL, (LPWSTR)filename.c_str(), NULL, 0, &info, NULL);
			Assert::IsNotNull(handle);

			Assert::AreEqual((UINT64)524288, (UINT64)info.nSectorCount);
			Assert::AreEqual((DWORD)512, info.nSectorSize);
			Assert::AreEqual((DWORD)IIO_INIT_READ | IIO_INIT_THREADSAFE | IIO_INIT_UNALIGNED_OK | IIO_INIT_DISK, info.nFlags);

			Assert::IsNotNull(info.lpTextualDescr);
			std::wstring textDesc(info.lpTextualDescr);

			std::string sha1 = sha1sum(handle, info.nSectorCount * info.nSectorSize, 128 * 1024);
			std::string expectedSHA1 = "e8650e89b262cf0b4b73c025312488d5a6317a26";
			Assert::AreEqual(expectedSHA1, sha1);

			// We now test the map for sparse regions.

			// Data
			checkIsSparse(handle, 0x00000000, 32768, 0);
			// aff4:Zero
			checkIsSparse(handle, 0x00008000, 32768, IIO_SPARSE_DETECTED);
			checkIsSparse(handle, 0x01070000, 0x00090000, IIO_SPARSE_DETECTED);
			// Unknown
			checkIsSparse(handle, 0x01100000, 32768, IIO_SPARSE_DETECTED);
			checkIsSparse(handle, 0x01100000 + 0x03e00000 - 32768, 32768, IIO_SPARSE_DETECTED);
			// End of disk
			checkIsSparse(handle, 0x0fe00000, 0x00200000, IIO_SPARSE_DETECTED);
			
			// Overlapping Data + Zero
			checkIsSparse(handle, 0x00000000, 65536, 0);

			// Overlapping Zero + Unknown.
			checkIsSparse(handle, 0x01100000 - 32768, 65536, IIO_SPARSE_DETECTED);

			// Symbolic (0xFF)
			checkIsSparse(handle, 0x04f80000, 32768, 0);

			// Check for subregions...
			checkIsSparse(handle, 0x00000000, 16, 0);
			checkIsSparse(handle, 0x00000000, 512, 0);
			checkIsSparse(handle, 0x00008000 + 100, 32768 - 100, IIO_SPARSE_DETECTED);
			checkIsSparse(handle, 0x01100000 + 1, 16, IIO_SPARSE_DETECTED);
			checkIsSparse(handle, 0x0fe00000, 0x00200000 - 100, IIO_SPARSE_DETECTED);
			checkIsSparse(handle, 0x10000000 - 100, 100, IIO_SPARSE_DETECTED);
			checkIsSparse(handle, 0x00008000 - 16, 32, 0);

			// Known sparse regions.
			/*
			This is the map from the Base-Allocated.aff4 image. we can construct the tests based on this.
			0 = > aff4://fce3df71-dce8-4a17-af67-36bed58f25c9
			1 = > http://aff4.org/Schema#Zero
			2 = > http://aff4.org/Schema#UnknownData
			3 = > http://aff4.org/Schema#SymbolicStreamFF
			4 = > http://aff4.org/Schema#SymbolicStream61
			[0x00000000:0x00008000] = > stream 0[0x00000000:0x00008000]
				[0x00008000:0x00008000] = > stream 1[0x00008000:0x00008000]
				[0x00010000:0x00020000] = > stream 0[0x00008000:0x00020000]
				[0x00030000:0x00008000] = > stream 0[0x00030000:0x00008000]
				[0x00038000:0x00008000] = > stream 0[0x00028000:0x00008000]
				[0x00040000:0x00df8000] = > stream 1[0x00040000:0x00df8000]
				[0x00e38000:0x00040000] = > stream 0[0x00080000:0x00040000]
				[0x00e78000:0x00008000] = > stream 0[0x00118000:0x00008000]
				[0x00e80000:0x00008000] = > stream 0[0x000c8000:0x00008000]
				[0x00e88000:0x00008000] = > stream 0[0x000c0000:0x00008000]
				[0x00e90000:0x00008000] = > stream 0[0x000d8000:0x00008000]
				[0x00e98000:0x00008000] = > stream 0[0x000d0000:0x00008000]
				[0x00ea0000:0x00008000] = > stream 0[0x000e0000:0x00008000]
				[0x00ea8000:0x00008000] = > stream 0[0x00120000:0x00008000]
				[0x00eb0000:0x00030000] = > stream 0[0x000e8000:0x00030000]
				[0x00ee0000:0x00068000] = > stream 0[0x00128000:0x00068000]
				[0x00f48000:0x00008000] = > stream 0[0x00198000:0x00008000]
				[0x00f50000:0x00008000] = > stream 0[0x00190000:0x00008000]
				[0x00f58000:0x00008000] = > stream 0[0x001a0000:0x00008000]
				[0x00f60000:0x00010000] = > stream 0[0x001b0000:0x00010000]
				[0x00f70000:0x00008000] = > stream 0[0x001a8000:0x00008000]
				[0x00f78000:0x00010000] = > stream 0[0x001c0000:0x00010000]
				[0x00f88000:0x00008000] = > stream 0[0x001d8000:0x00008000]
				[0x00f90000:0x00008000] = > stream 0[0x001d0000:0x00008000]
				[0x00f98000:0x00008000] = > stream 0[0x001e8000:0x00008000]
				[0x00fa0000:0x00008000] = > stream 0[0x001e0000:0x00008000]
				[0x00fa8000:0x00018000] = > stream 0[0x001f0000:0x00018000]
				[0x00fc0000:0x00008000] = > stream 0[0x00210000:0x00008000]
				[0x00fc8000:0x00008000] = > stream 0[0x00208000:0x00008000]
				[0x00fd0000:0x00008000] = > stream 0[0x00220000:0x00008000]
				[0x00fd8000:0x00008000] = > stream 0[0x00218000:0x00008000]
				[0x00fe0000:0x00020000] = > stream 0[0x00228000:0x00020000]
				[0x01000000:0x00008000] = > stream 0[0x003b8000:0x00008000]
				[0x01008000:0x00008000] = > stream 0[0x00290000:0x00008000]
				[0x01010000:0x00008000] = > stream 0[0x00250000:0x00008000]
				[0x01018000:0x00008000] = > stream 0[0x00248000:0x00008000]
				[0x01020000:0x00008000] = > stream 0[0x002c0000:0x00008000]
				[0x01028000:0x00008000] = > stream 0[0x00288000:0x00008000]
				[0x01030000:0x00008000] = > stream 0[0x00270000:0x00008000]
				[0x01038000:0x00010000] = > stream 0[0x00258000:0x00010000]
				[0x01048000:0x00008000] = > stream 0[0x00278000:0x00008000]
				[0x01050000:0x00008000] = > stream 0[0x002c8000:0x00008000]
				[0x01058000:0x00008000] = > stream 0[0x00268000:0x00008000]
				[0x01060000:0x00008000] = > stream 0[0x002b8000:0x00008000]
				[0x01068000:0x00008000] = > stream 0[0x00280000:0x00008000]
				[0x01070000:0x00090000] = > stream 1[0x01070000:0x00090000]
				[0x01100000:0x03e00000] = > stream 2[0x01100000:0x03e00000]
				[0x04f00000:0x00050000] = > stream 1[0x04f00000:0x00050000]
				[0x04f50000:0x00008000] = > stream 0[0x003a8000:0x00008000]
				[0x04f58000:0x00008000] = > stream 0[0x003a0000:0x00008000]
				[0x04f60000:0x00020000] = > stream 0[0x00298000:0x00020000]
				[0x04f80000:0x00258000] = > stream 3[0x04f80000:0x00258000]
				[0x051d8000:0x00008000] = > stream 0[0x002d0000:0x00008000]
				[0x051e0000:0x00038000] = > stream 1[0x051e0000:0x00038000]
				[0x05218000:0x00008000] = > stream 0[0x002d8000:0x00008000]
				[0x05220000:0x000f8000] = > stream 1[0x05220000:0x000f8000]
				[0x05318000:0x00008000] = > stream 0[0x002e8000:0x00008000]
				[0x05320000:0x00008000] = > stream 0[0x002e0000:0x00008000]
				[0x05328000:0x000b0000] = > stream 0[0x002f0000:0x000b0000]
				[0x053d8000:0x00088000] = > stream 1[0x053d8000:0x00088000]
				[0x05460000:0x00010000] = > stream 0[0x00038000:0x00010000]
				[0x05470000:0x00008000] = > stream 0[0x00060000:0x00008000]
				[0x05478000:0x00008000] = > stream 0[0x00048000:0x00008000]
				[0x05480000:0x00008000] = > stream 0[0x00058000:0x00008000]
				[0x05488000:0x00008000] = > stream 0[0x00050000:0x00008000]
				[0x05490000:0x00018000] = > stream 0[0x00068000:0x00018000]
				[0x054a8000:0x00058000] = > stream 1[0x054a8000:0x00058000]
				[0x05500000:0x0a800000] = > stream 2[0x05500000:0x0a800000]
				[0x0fd00000:0x00008000] = > stream 1[0x0fd00000:0x00008000]
				[0x0fd08000:0x00008000] = > stream 0[0x003b0000:0x00008000]
				[0x0fd10000:0x00008000] = > stream 4[0x0fd10000:0x00008000]
				[0x0fd18000:0x00008000] = > stream 0[0x003c0000:0x00008000]
				[0x0fd20000:0x000e0000] = > stream 1[0x0fd20000:0x000e0000]
				[0x0fe00000:0x00200000] = > stream 2[0x0fe00000:0x00200000]
				*/

			IIO_Done(handle, info.lpTextualDescr);
		}

		TEST_METHOD(testMapSize)
		{
			std::wstring filename(L"D:\\Test2\\ToshibaUSB_PhysicalDrive3_C.aff4");
			if (PathFileExistsW(filename.c_str()) == FALSE) {
				Assert::IsTrue(true);
				return;
			}
			ImageInfo info;
			info.nSize = sizeof(ImageInfo);
			info.nFlags = 0;
			info.nSectorCount = 0;
			info.nSectorSize = 0;
			info.lpTextualDescr = NULL;

			PVOID handle = IIO_Init(NULL, (LPWSTR)filename.c_str(), NULL, 0, &info, NULL);
			Assert::IsNotNull(handle);

			Assert::AreEqual((UINT64)15137280, (UINT64)info.nSectorCount);
			Assert::AreEqual((DWORD)512, info.nSectorSize);
			Assert::AreEqual((DWORD)IIO_INIT_READ | IIO_INIT_THREADSAFE | IIO_INIT_UNALIGNED_OK | IIO_INIT_DISK, info.nFlags);

			Assert::IsNotNull(info.lpTextualDescr);
			std::wstring textDesc(info.lpTextualDescr);
			std::wcout << textDesc << std::endl;

			std::string sha1 = sha1sum(handle, info.nSectorCount * info.nSectorSize, 128 * 1024);
			std::string expectedSHA1 = "1865bc3df2cc5b4ed60fc86909fd534362181bed";
			Assert::AreEqual(expectedSHA1, sha1);

			IIO_Done(handle, info.lpTextualDescr);
		}

	};
}