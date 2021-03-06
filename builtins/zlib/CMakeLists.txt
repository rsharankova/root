project(ZLIB C)

set(ZLIB_PUBLIC_HEADERS
    zconf.h
    zlib.h
)

set(ZLIB_PRIVATE_HEADERS
    crc32.h
    deflate.h
    gzguts.h
    inffast.h
    inffixed.h
    inflate.h
    inftrees.h
    trees.h
    zutil.h
)

set(ZLIB_SOURCES
    adler32.c
    compress.c
    crc32.c
    deflate.c
    gzclose.c
    gzlib.c
    gzread.c
    gzwrite.c
    inflate.c
    infback.c
    inftrees.c
    inffast.c
    trees.c
    uncompr.c
    zutil.c
)

unset(ZLIB_FOUND CACHE)
unset(ZLIB_FOUND PARENT_SCOPE)
set(ZLIB_FOUND TRUE CACHE BOOL "" FORCE)

file(STRINGS zlib.h ZLIB_H REGEX "^#define ZLIB_VERSION \"[^\"]*\"$")
string(REGEX REPLACE "^.*ZLIB_VERSION \"([0-9]+).*$" "\\1" ZLIB_VERSION_MAJOR "${ZLIB_H}")
string(REGEX REPLACE "^.*ZLIB_VERSION \"[0-9]+\\.([0-9]+).*$" "\\1" ZLIB_VERSION_MINOR  "${ZLIB_H}")
string(REGEX REPLACE "^.*ZLIB_VERSION \"[0-9]+\\.[0-9]+\\.([0-9]+).*$" "\\1" ZLIB_VERSION_PATCH "${ZLIB_H}")
set(ZLIB_VERSION_STRING "${ZLIB_VERSION_MAJOR}.${ZLIB_VERSION_MINOR}.${ZLIB_VERSION_PATCH}")

set(ZLIB_VERSION_TWEAK "")
if("${ZLIB_H}" MATCHES "ZLIB_VERSION \"[0-9]+\\.[0-9]+\\.[0-9]+\\.([0-9]+)")
  set(ZLIB_VERSION_TWEAK "${CMAKE_MATCH_1}")
  string(APPEND ZLIB_VERSION_STRING ".${ZLIB_VERSION_TWEAK}")
endif()

set(ZLIB_VERSION ${ZLIB_VERSION_STRING} CACHE INTERNAL "")
set(ZLIB_VERSION_STRING ${ZLIB_VERSION_STRING} CACHE INTERNAL "")

set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")
set(ZLIB_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

add_library(ZLIB STATIC ${ZLIB_PUBLIC_HEADERS} ${ZLIB_PRIVATE_HEADERS} ${ZLIB_SOURCES})
target_include_directories(ZLIB INTERFACE $<BUILD_INTERFACE:${ZLIB_INCLUDE_DIR}>)
target_compile_options(ZLIB PRIVATE -fvisibility=hidden)

add_library(ZLIB::ZLIB ALIAS ZLIB)

set(ZLIB_LIBRARY $<TARGET_FILE:ZLIB> CACHE INTERNAL "")
set(ZLIB_LIBRARIES ZLIB::ZLIB CACHE INTERNAL "")

if(DEFINED ZLIB_LIBRARY_DEBUG)
  set(ZLIB_LIBRARY_DEBUG ${ZLIB_LIBRARY} CACHE INTERNAL "")
endif()

if(DEFINED ZLIB_LIBRARY_RELEASE)
  set(ZLIB_LIBRARY_RELEASE ${ZLIB_LIBRARY} CACHE INTERNAL "")
endif()

set_property(GLOBAL APPEND PROPERTY ROOT_BUILTIN_TARGETS ZLIB::ZLIB)
