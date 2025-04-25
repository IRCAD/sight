# Beware, at least for windows we need to use the same PCH than our obj library
sight_add_target(core_detail_ut TYPE TEST)

# We need this one to test the location
target_compile_definitions(core_detail_ut PRIVATE MODULE_BIN_PREFIX="${CMAKE_INSTALL_BINDIR}")
target_compile_definitions(core_detail_ut PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")
target_compile_definitions(core_detail_ut PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")

if(WIN32)
    # Avoid to import symbols from core
    target_compile_definitions(core_detail_ut PRIVATE "CORE_EXPORTS")
endif()

# Link with the library objects instead of classical shared library
target_link_libraries(core_detail_ut PUBLIC core_obj)

target_link_libraries(core_detail_ut PRIVATE CppUnit)

target_link_libraries(core_detail_ut PRIVATE LibXml2::LibXml2)

if(UNIX)
    target_link_libraries(core_detail_ut PRIVATE dl)
endif()

add_dependencies(core_detail_ut utest module_utest)
