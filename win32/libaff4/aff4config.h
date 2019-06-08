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

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "info@evimetry.com"

/* Define to the full name of this package. */
#define PACKAGE_NAME "aff4"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "aff4 2.0.3"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "aff4"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.0.3"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

#ifdef _DEBUG
	#ifndef DEBUG
		#define DEBUG 1
	#endif
#else
	#ifndef DEBUG
		#define DEBUG 0
	#endif
#endif


/* Enable PRIx64, et al. macros on all platforms */
#ifndef __STDC_FORMAT_MACROS
 #define __STDC_FORMAT_MACROS 1
#endif
