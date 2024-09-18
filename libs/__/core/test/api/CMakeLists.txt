sight_add_target(core_ut TYPE TEST)

if(UNIX)
    target_compile_definitions(${SIGHT_TARGET} PRIVATE BOOST_LIB_DIR="/lib/x86_64-linux-gnu")
    target_link_libraries(${SIGHT_TARGET} PUBLIC "dl")
elseif(WIN32)
    target_compile_definitions(${SIGHT_TARGET} PRIVATE BOOST_LIB_DIR="${SIGHT_VCPKG_ROOT_DIR}/bin")
endif()

target_compile_definitions(
    ${SIGHT_TARGET} PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}" MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}"
                            SIGHT_VERSION="${PROJECT_VERSION}" BOOST_MAJOR_VERSION="${Boost_VERSION}"
)

find_package(PkgConfig QUIET)

if(PKGCONFIG_FOUND)
    pkg_check_modules(ZSTD libzstd)
endif()

if(NOT WIN32 AND ZSTD_FOUND)
    target_include_directories(${SIGHT_TARGET} SYSTEM PRIVATE ${ZSTD_INCLUDE_DIRS})
    target_link_libraries(${SIGHT_TARGET} PRIVATE ${ZSTD_LINK_LIBRARIES})
else()
    find_package(zstd CONFIG REQUIRED)
    target_link_libraries(${SIGHT_TARGET} PRIVATE zstd::libzstd_shared)
endif()

find_package(OpenSSL COMPONENTS Crypto)
target_link_libraries(${SIGHT_TARGET} PRIVATE OpenSSL::Crypto)

add_dependencies(${SIGHT_TARGET} utest module_utest)

target_link_libraries(${SIGHT_TARGET} PUBLIC core CppUnit)
