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

#include "aff4.h"
#include "aff4-c.h"

namespace AFF4UnitTests
{		
	TEST_CLASS(TestVersion)
	{
	public:
		
		TEST_METHOD(testVersionString) {
			std::string version = aff4::getVersionString();
			std::cout << version << std::endl;
		}

		TEST_METHOD(testCVersionString) {
			const char* version = AFF4_version();
			std::string v(version);
			std::cout << version << std::endl;
		}

	};
}