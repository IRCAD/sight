sight_add_target( io_zip TYPE LIBRARY WARNINGS_AS_ERRORS OFF)
target_link_libraries(io_zip PUBLIC core)

include(CheckIncludeFile)
include(CheckTypeSize)
include(CheckFunctionExists)

# Check for system includes
check_include_file(stdint.h   HAVE_STDINT_H)
if(HAVE_STDINT_H)
    target_compile_definitions(io_zip PRIVATE "-DHAVE_STDINT_H")
endif()

check_include_file(inttypes.h HAVE_INTTYPES_H)
if(HAVE_INTTYPES_H)
    target_compile_definitions(io_zip PRIVATE "-DHAVE_INTTYPES_H")
endif()

# Check for large file support
check_type_size(off64_t OFF64_T)
if(HAVE_OFF64_T)
    target_compile_definitions(io_zip PRIVATE "-D__USE_LARGEFILE64")
    target_compile_definitions(io_zip PRIVATE "-D_LARGEFILE64_SOURCE")
endif()

# Check for fseeko support
check_function_exists(fseeko HAVE_FSEEKO)
if(NOT HAVE_FSEEKO)
    target_compile_definitions(io_zip PRIVATE "-DNO_FSEEKO")
endif()

# ZLIB
find_package(ZLIB QUIET REQUIRED)
target_link_libraries(io_zip PRIVATE ZLIB::ZLIB)
target_compile_definitions(io_zip PRIVATE "-DHAVE_ZLIB")

if(ZLIB_COMPAT)
    target_compile_definitions(io_zip PRIVATE "-DZLIB_COMPAT")
endif()

# ZSTD
find_package(PkgConfig QUIET)
if(PKGCONFIG_FOUND)
    pkg_check_modules(ZSTD libzstd)
endif()
if(ZSTD_FOUND)
    target_include_directories(io_zip SYSTEM PRIVATE ${ZSTD_INCLUDE_DIRS})
    target_link_libraries(io_zip PRIVATE ${ZSTD_LIBRARIES})
else()
    find_package(zstd CONFIG REQUIRED)
    target_link_libraries(io_zip PRIVATE zstd::libzstd_shared)
endif()
target_compile_definitions(io_zip PRIVATE "-DHAVE_ZSTD")

# OpenSSL
find_package(OpenSSL QUIET REQUIRED COMPONENTS Crypto)
target_link_libraries(io_zip PRIVATE OpenSSL::Crypto)

# Windows specific
if(WIN32)
    target_compile_definitions(io_zip PRIVATE "-D_CRT_SECURE_NO_DEPRECATE")
endif()

# Unix specific
if(UNIX)
    # ICONV
    find_package(Iconv QUIET REQUIRED)
    target_include_directories(io_zip SYSTEM PRIVATE ${Iconv_INCLUDE_DIRS})
    target_link_libraries(io_zip PRIVATE ${Iconv_LIBRARIES})
    target_compile_definitions(io_zip PRIVATE "-DHAVE_ICONV")
endif()

# Setup predefined macros
target_compile_definitions(io_zip PRIVATE "-DMZ_ZIP_SIGNING")

# Include WinZIP AES encryption
target_compile_definitions(io_zip PRIVATE "-DHAVE_WZAES")

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
