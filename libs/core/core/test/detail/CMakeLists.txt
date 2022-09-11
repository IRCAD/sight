# Beware, at least for windows we need to use the same PCH than our obj library
sight_add_target(coreDetailTest TYPE TEST PCH core_obj)

# We need this one to test the location
target_compile_definitions(coreDetailTest PRIVATE MODULE_BIN_PREFIX="${CMAKE_INSTALL_BINDIR}")
target_compile_definitions(coreDetailTest PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")
target_compile_definitions(coreDetailTest PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")

if(WIN32)
    # Avoid to import symbols from core
    target_compile_definitions(coreDetailTest PRIVATE "CORE_EXPORTS")
endif()

# Link with the library objects instead of classical shared library
target_link_libraries(coreDetailTest PUBLIC core_obj)

target_link_libraries(coreDetailTest PRIVATE CppUnit)

target_link_libraries(coreDetailTest PRIVATE LibXml2::LibXml2)

if(UNIX)
    target_link_libraries(coreDetailTest PRIVATE dl)
endif()

add_dependencies(coreDetailTest utest module_utest)
