sight_add_target(core_ut TYPE DOCTEST)

# We need this one to test the location
target_compile_definitions(${SIGHT_TARGET} PRIVATE MODULE_BIN_PREFIX="${CMAKE_INSTALL_BINDIR}")
target_compile_definitions(${SIGHT_TARGET} PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")
target_compile_definitions(${SIGHT_TARGET} PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")

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

target_link_libraries(${SIGHT_TARGET} PRIVATE core)

add_dependencies(${SIGHT_TARGET} utest module_utest)
