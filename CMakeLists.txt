PROJECT(aff4-cpp-lite)

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

FIND_PACKAGE(Raptor2 REQUIRED)

FIND_PACKAGE(Snappy REQUIRED)

FIND_PACKAGE(ZLIB REQUIRED)

FIND_PACKAGE(OpenSSL REQUIRED)

FIND_PACKAGE(LZ4 REQUIRED)

add_definitions(-DUNICODE -DLIBAFF4_EXPORTS=0 -DRAPTOR_STATIC)

SET(SOURCES
  win32/libaff4/aff4config.h
  win32/libaff4/src/stdafx.h

  src/aff4.cc
  src/aff4-c.cc
  src/AFF4Lexicon.cc
  src/RDFValue.cc
  src/AFF4Containers.cc
  
  src/utils/StringUtil.cc src/utils/StringUtil.h
  src/utils/FileUtil.h
  src/utils/Cache.h
  src/utils/PortableEndian.h
  src/rdf/Model.cc src/rdf/Model.h
  src/resource/AFF4Resource.cc src/resource/AFF4Resource.h
  src/zip/Zip.cc src/zip/Zip.h
  src/zip/ZipStream.cc src/zip/ZipStream.h
  src/container/AFF4ZipContainer.cc src/container/AFF4ZipContainer.h
  src/image/AFF4Image.cc src/image/AFF4Image.h
  src/stream/ImageStreamFactory.cc src/stream/ImageStreamFactory.h
  src/stream/RepeatedImageStream.cc src/stream/RepeatedImageStream.h
  src/stream/SymbolicImageStream.cc src/stream/SymbolicImageStream.h
  src/stream/ImageStream.cc src/stream/ImageStream.h
  src/stream/MapStream.cc src/stream/MapStream.h
  src/stream/struct/BevvyIndex.cc src/stream/struct/BevvyIndex.h
  src/stream/struct/BevvyIndexLoader.cc src/stream/struct/BevvyIndexLoader.h
  src/stream/struct/ChunkLoader.cc src/stream/struct/ChunkLoader.h
  src/stream/struct/ImageStreamPoint.h
  src/stream/struct/MapEntryPoint.h
  src/map/AFF4Map.cc src/map/AFF4Map.h
  src/codec/CompressionCodec.cc src/codec/CompressionCodec.h
  src/codec/NullCompression.cc src/codec/NullCompression.h
  src/codec/DeflateCompression.cc src/codec/DeflateCompression.h
  src/codec/ZlibCompression.cc src/codec/ZlibCompression.h
  src/codec/LZ4Compression.cc src/codec/LZ4Compression.h
  src/codec/SnappyCompression.cc src/codec/SnappyCompression.h
  src/resolver/LightResolver.cc src/resolver/LightResolver.h
)

ADD_LIBRARY(aff4-cpp-lite ${SOURCES})

target_include_directories(aff4-cpp-lite PUBLIC
  win32/libaff4/src
  win32/libaff4
  
  src
  src/codec
  src/container
  src/image
  src/map
  src/rdf
  src/resolver
  src/resource
  src/stream
  src/stream/struct
  src/utils
  src/zip
)

TARGET_LINK_LIBRARIES(aff4-cpp-lite PRIVATE Raptor2)
TARGET_LINK_LIBRARIES(aff4-cpp-lite PRIVATE Snappy)
TARGET_LINK_LIBRARIES(aff4-cpp-lite PRIVATE OpenSSL)
TARGET_LINK_LIBRARIES(aff4-cpp-lite PRIVATE ZLIB)
TARGET_LINK_LIBRARIES(aff4-cpp-lite PRIVATE LZ4::LZ4)

FILE(WRITE ${CMAKE_CURRENT_BINARY_DIR}/libaff4.pc
"prefix=${CMAKE_INSTALL_PREFIX}
exec_prefix=\${prefix}
libdir=\${exec_prefix}/${CMAKE_INSTALL_LIBDIR}
includedir=\${prefix}/${CMAKE_INSTALL_INCLUDEDIR}

Name: aff4-cpp-lite library
Description: aff4-cpp-lite library
Version: ${VERSION}
Libs: -L\${libdir} -libaff4
Cflags: -I\${includedir}
")

SET(INSTALL_HEADERS
    ${CMAKE_CURRENT_BINARY_DIR}/src/aff4.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/aff4-c.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/AFF4Containers.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/AFF4Defaults.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/AFF4Lexicon.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/IAFF4Container.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/IAFF4Image.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/IAFF4Map.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/IAFF4Resolver.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/IAFF4Resource.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/IAFF4Stream.h
    ${CMAKE_CURRENT_BINARY_DIR}/src/RDFValue.h
)

if (WIN32)
    SET(INSTALL_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/win32/libaff4/src/stdafx.h
        ${CMAKE_CURRENT_SOURCE_DIR}/win32/libaff4/src/targetver.h
        ${INSTALL_HEADERS}
    )
endif()

INSTALL(FILES
    ${INSTALL_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

INSTALL(
    TARGETS aff4-cpp-lite
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)