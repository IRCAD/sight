sight_add_target( coreDetailTest TYPE TEST WARNINGS_AS_ERRORS ON )


# We need this one to test the location
target_compile_definitions(${FWPROJECT_NAME} PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")
target_compile_definitions(${FWPROJECT_NAME} PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")

# Link with the library objects instead of classical shared library
target_link_libraries(${FWPROJECT_NAME} PUBLIC core_obj)

find_package(CppUnit QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE CppUnit)

if(UNIX)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE dl)
endif()

add_dependencies(${FWPROJECT_NAME} 
                 utest
                 module_utest
)
