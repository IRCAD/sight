find_package(Filesystem REQUIRED)
find_package(Boost QUIET COMPONENTS system date_time iostreams log log_setup REQUIRED)
find_package(LibXml2 QUIET REQUIRED)
find_package(OpenSSL COMPONENTS Crypto)

# ZSTD
find_package(PkgConfig QUIET)

if(PKGCONFIG_FOUND)
    pkg_check_modules(ZSTD libzstd)
endif()

if(WIN32 OR NOT ZSTD_FOUND)
    find_package(zstd CONFIG REQUIRED)
endif()
