sight_add_target(core_ut TYPE TEST)

if(UNIX)
    target_compile_definitions(core_ut PRIVATE BOOST_LIB_DIR="/lib/x86_64-linux-gnu")
    target_link_libraries(core_ut PUBLIC "dl")
elseif(WIN32)
    target_compile_definitions(core_ut PRIVATE BOOST_LIB_DIR="${SIGHT_VCPKG_ROOT_DIR}/bin")
endif()

target_compile_definitions(
    core_ut PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}" MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}"
                    SIGHT_VERSION="${PROJECT_VERSION}" BOOST_MAJOR_VERSION="${Boost_VERSION}"
)

add_dependencies(core_ut utest module_utest sightlog)

target_link_libraries(core_ut PUBLIC core CppUnit)
