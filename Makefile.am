AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}
EXTRA_DIST = autogen.sh src/aff4.rc libaff4.pc COPYING README.md
SUBDIRS = src tests docs examples

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=libaff4.pc

# Netbeans support for cppunit testing.

# Test Directory
TESTDIR=tests

# build tests
build-tests: check
	
# Run Test Targets
test: 
if HAVE_CPPUNIT
	@if [ "${TEST}" = "" ]; \
	then  \
	    ${TESTDIR}/container || true; \
	    ${TESTDIR}/image || true; \
	    ${TESTDIR}/streams || true; \
	    ${TESTDIR}/compression || true; \
	    ${TESTDIR}/resolver || true; \
		${TESTDIR}/cache || true; \
		${TESTDIR}/version || true; \
	else  \
	    ./${TEST} || true; \
	fi
endif
