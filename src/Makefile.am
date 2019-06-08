
AUTOMAKE_OPTIONS=subdir-objects
ACLOCAL_AMFLAGS=${ACLOCAL_FLAGS}

lib_LTLIBRARIES = libaff4.la

libaff4_includedir = $(includedir)/aff4/
libaff4_include_HEADERS = \
	aff4.h \
	aff4-c.h \
	AFF4Defaults.h \
	AFF4Lexicon.h \
	IAFF4Resource.h \
	IAFF4Container.h \
	IAFF4Resolver.h \
	IAFF4Image.h \
	IAFF4Map.h \
	IAFF4Stream.h \
	AFF4Containers.h \
	RDFValue.h 
	
libaff4_la_SOURCES = \
	aff4.cc \
	aff4-c.cc \
	AFF4Lexicon.cc \
	RDFValue.cc \
	AFF4Containers.cc \
	utils/StringUtil.cc utils/StringUtil.h \
	utils/FileUtil.h \
	utils/Cache.h \
	utils/PortableEndian.h \
	rdf/Model.cc rdf/Model.h \
	resource/AFF4Resource.cc resource/AFF4Resource.h \
	zip/Zip.cc zip/Zip.h \
	zip/ZipStream.cc zip/ZipStream.h \
	container/AFF4ZipContainer.cc container/AFF4ZipContainer.h \
	image/AFF4Image.cc image/AFF4Image.h \
	stream/ImageStreamFactory.cc stream/ImageStreamFactory.h \
	stream/RepeatedImageStream.cc stream/RepeatedImageStream.h \
	stream/SymbolicImageStream.cc stream/SymbolicImageStream.h \
	stream/ImageStream.cc stream/ImageStream.h \
	stream/MapStream.cc stream/MapStream.h \
	stream/struct/BevvyIndex.cc stream/struct/BevvyIndex.h \
	stream/struct/BevvyIndexLoader.cc stream/struct/BevvyIndexLoader.h \
	stream/struct/ChunkLoader.cc stream/struct/ChunkLoader.h \
	stream/struct/ImageStreamPoint.h \
	stream/struct/MapEntryPoint.h \
	map/AFF4Map.cc map/AFF4Map.h \
	codec/CompressionCodec.cc codec/CompressionCodec.h \
	codec/NullCompression.cc codec/NullCompression.h \
	codec/DeflateCompression.cc codec/DeflateCompression.h \
	codec/ZlibCompression.cc codec/ZlibCompression.h \
	codec/LZ4Compression.cc codec/LZ4Compression.h \
	codec/SnappyCompression.cc codec/SnappyCompression.h \
	resolver/LightResolver.cc resolver/LightResolver.h 

if HAVE_WINDRES
libaff4_la_SOURCES += aff4.rc
endif

.rc.o:
	windres $^ -o $@
%.o : %.rc
	windres $^ -o $@

if DEBUG
DBG = -g3 -O0
else
DBG = 
endif

EXTRA_DIST=libaff4.sym

# ---------------------------------------------------------------------------
# Advanced information about versioning:
#   * "Writing shared libraries" by Mike Hearn
#      http://plan99.net/~mike/writing-shared-libraries.html
#   * libtool.info chapter "Versioning"
#   * libtool.info chapter "Updating library version information"
# ---------------------------------------------------------------------------
# Versioning:
#  - CURRENT (Major):  Increment if the interface has changes. AGE is always
#                      *changed* at the same time.
#  - AGE (Micro):      Increment if any interfaces have been added; set to 0
#		         if any interfaces have been removed. Removal has
#                      precedence over adding, so set to 0 if both happened.
#                      It denotes upward compatibility.
#  - REVISION (Minor): Increment any time the source changes; set to
#			 0 if you incremented CURRENT.
#
#  To summarize. Any interface *change* increment CURRENT. If that interface
#  change does not break upward compatibility (ie it is an addition),
#  increment AGE, Otherwise AGE is reset to 0. If CURRENT has changed,
#  REVISION is set to 0, otherwise REVISION is incremented.
# ---------------------------------------------------------------------------
CURRENT=2
AGE=0
REVISION=0
SOVERSION=$(CURRENT):$(REVISION):$(AGE)
LT_CURRENT_MINUS_AGE=`expr $(CURRENT) - $(AGE)`

libaff4_la_INCLUDES = -I./resource \
	-I./zip \
	-I./container \
	-I./utils \
	-I./rdf \
	-I./image \
	-I./stream \
	-I./stream/struct \
	-I./codec \
	-I./resolver \
	-I./map \
	${DBG}
libaff4_la_CXXFLAGS = ${libaff4_la_INCLUDES} ${ZLIB_CFLAGS} ${RAPTOR2_CFLAGS} ${LZ4_CFLAGS} 
libaff4_la_LIBADD = ${ZLIB_LIBS} ${RAPTOR2_LIBS} ${LZ4_LIBS} 
libaff4_la_LDFLAGS=@LDFLAGS@ -no-undefined -export-symbols $(srcdir)/libaff4.sym -version-info $(SOVERSION) 

datadir = @datadir@
datarootdir= @datarootdir@
localedir = @localedir@
DEFS = -DLOCALEDIR=\"$(localedir)\" -DPACKAGE_DATADIR=\"$(pkgdatadir)\" @DEFS@
