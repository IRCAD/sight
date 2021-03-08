sight_add_target( coreDetailTest TYPE TEST WARNINGS_AS_ERRORS ON )


# We need this one to test the location
target_compile_definitions(coreDetailTest PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")
target_compile_definitions(coreDetailTest PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")

# Link with the library objects instead of classical shared library
target_link_libraries(coreDetailTest PUBLIC core_obj)

target_link_libraries(coreDetailTest PRIVATE CppUnit)

if(UNIX)
    target_link_libraries(coreDetailTest PRIVATE dl)
endif()

add_dependencies(coreDetailTest 
                 utest
                 module_utest
)
