file(GLOB MINIZIP_SOURCES *.c *.h)
add_library(minizip STATIC ${MINIZIP_SOURCES})
target_compile_options(minizip PRIVATE "$<$<CXX_COMPILER_ID:GNU,Clang>:-w>$<$<CXX_COMPILER_ID:MSVC>:/w>")

include(CheckIncludeFile)
include(CheckTypeSize)
include(CheckFunctionExists)

# Check for system includes
check_include_file(stdint.h HAVE_STDINT_H)
if(HAVE_STDINT_H)
    target_compile_definitions(minizip PRIVATE "-DHAVE_STDINT_H")
endif()

check_include_file(inttypes.h HAVE_INTTYPES_H)
if(HAVE_INTTYPES_H)
    target_compile_definitions(minizip PRIVATE "-DHAVE_INTTYPES_H")
endif()

# Check for large file support
check_type_size(off64_t OFF64_T)
if(HAVE_OFF64_T)
    target_compile_definitions(minizip PRIVATE "-D__USE_LARGEFILE64")
    target_compile_definitions(minizip PRIVATE "-D_LARGEFILE64_SOURCE")
endif()

# Check for fseeko support
check_function_exists(fseeko HAVE_FSEEKO)
if(NOT HAVE_FSEEKO)
    target_compile_definitions(minizip PRIVATE "-DNO_FSEEKO")
endif()

# ZLIB
find_package(ZLIB QUIET REQUIRED)
target_link_libraries(minizip PRIVATE ZLIB::ZLIB)
target_compile_definitions(minizip PRIVATE "-DHAVE_ZLIB")
target_compile_definitions(minizip PRIVATE "-DZLIB_COMPAT")

# ZSTD
find_package(PkgConfig QUIET)
if(PKGCONFIG_FOUND)
    pkg_check_modules(ZSTD libzstd)
endif()
if(ZSTD_FOUND)
    target_include_directories(minizip SYSTEM PRIVATE ${ZSTD_INCLUDE_DIRS})
    target_link_libraries(minizip PRIVATE ${ZSTD_LINK_LIBRARIES})
else()
    find_package(zstd CONFIG REQUIRED)
    target_link_libraries(minizip PRIVATE zstd::libzstd_shared)
endif()
target_compile_definitions(minizip PRIVATE "-DHAVE_ZSTD")

# OpenSSL
find_package(OpenSSL QUIET REQUIRED COMPONENTS Crypto)
target_link_libraries(minizip PRIVATE OpenSSL::Crypto)

# Windows specific
if(WIN32)
    target_compile_definitions(minizip PRIVATE "-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE")
endif()

# Unix specific
if(UNIX)
    # ICONV
    find_package(Iconv QUIET REQUIRED)
    target_include_directories(minizip SYSTEM PRIVATE ${Iconv_INCLUDE_DIRS})
    target_link_libraries(minizip PRIVATE ${Iconv_LIBRARIES})
    target_compile_definitions(minizip PRIVATE "-DHAVE_ICONV")
endif()

# Setup predefined macros
target_compile_definitions(minizip PRIVATE "-DMZ_ZIP_SIGNING")

# Include WinZIP AES encryption
target_compile_definitions(minizip PRIVATE "-DHAVE_WZAES")
