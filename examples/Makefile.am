AUTOMAKE_OPTIONS=subdir-objects
ACLOCAL_AMFLAGS=${ACLOCAL_FLAGS}

if HAVE_OPENSSL

bin_PROGRAMS = aff4-info aff4-digest  aff4-extract

aff4_info_SOURCES= \
	info.cc utils.cc utils.h

aff4_extract_SOURCES=  \
	extract.cc utils.cc utils.h

aff4_digest_SOURCES=  \
	digest.cc utils.cc utils.h

AM_CPPFLAGS=-I$(top_builddir)/src \
	$(SSL_CFLAGS) $(ZLIB_CFLAGS) $(RAPTOR2_CFLAGS) $(LZ4_CFLAGS)
	
LDADD=../src/.libs/libaff4.a $(RAPTOR2_LIBS) $(SSL_LIBS) $(ZLIB_LIBS) $(RAPTOR2_LIBS) $(LZ4_LIBS) -ldl
#LDADD=-l../src/libaff4.la $(RAPTOR2_LIBS) $(SSL_LIBS) $(ZLIB_LIBS) $(RAPTOR2_LIBS) $(LZ4_LIBS) -ldl

endif
