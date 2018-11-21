AUTOMAKE_OPTIONS=subdir-objects
ACLOCAL_AMFLAGS=${ACLOCAL_FLAGS}

EXTRA_DIST = resources

if HAVE_CPPUNIT
if HAVE_OPENSSL

check_PROGRAMS = version container image streams compression resolver cache

# VERSION CHECKS

version_SOURCES= \
  version.cc version.h \
  TestRunner.cc TestUtilities.cc TestUtilities.h

# CONTAINER TESTS

container_SOURCES= \
  container.cc container.h \
  TestRunner.cc TestUtilities.cc TestUtilities.h
  
# IMAGE TESTS

image_SOURCES= \
  image.cc image.h \
  TestRunner.cc TestUtilities.cc TestUtilities.h
  
# IMAGE STREAM TESTS

streams_SOURCES= \
  streams.cc streams.h \
  TestRunner.cc TestUtilities.cc TestUtilities.h
  
# COMPRESSION TESTS

compression_SOURCES= \
  compression.cc compression.h \
  TestRunner.cc TestUtilities.cc TestUtilities.h
  
# RESOLVER TESTS

resolver_SOURCES= \
  resolver.cc resolver.h \
  TestRunner.cc TestUtilities.cc TestUtilities.h
  
# CACHE TESTS

cache_SOURCES= \
  cacheTest.cc cacheTest.h \
  TestRunner.cc TestUtilities.cc TestUtilities.h

AM_CPPFLAGS=-I$(top_builddir)/src \
	-I$(top_builddir)/src/codec \
	-I$(top_builddir)/src/container \
	-I$(top_builddir)/src/image \
	-I$(top_builddir)/src/rdf \
	-I$(top_builddir)/src/resolver \
	-I$(top_builddir)/src/resource \
	-I$(top_builddir)/src/stream \
	-I$(top_builddir)/src/stream/struct \
	-I$(top_builddir)/src/utils \
	-I$(top_builddir)/src/zip \
	-I$(top_builddir)/src/map \
	$(CPPUNIT_CFLAGS) $(SSL_CFLAGS) $(ZLIB_CFLAGS) $(RAPTOR2_CFLAGS) $(LZ4_CFLAGS)
	
LDADD=../src/.libs/libaff4.a $(RAPTOR2_LIBS) $(CPPUNIT_LIBS) $(SSL_LIBS) $(ZLIB_LIBS) $(RAPTOR2_LIBS) $(LZ4_LIBS) -ldl 

endif
endif