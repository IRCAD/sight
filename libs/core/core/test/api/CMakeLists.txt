sight_add_target( coreTest TYPE TEST WARNINGS_AS_ERRORS ON )


if(UNIX)
    target_compile_definitions(coreTest PRIVATE CAMP_LIB_DIR="/lib/x86_64-linux-gnu")
    target_compile_definitions(coreTest PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")
    target_compile_definitions(coreTest PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")
    target_link_libraries(coreTest PUBLIC "dl")
endif()

add_dependencies(coreTest 
                 module_utest
)

target_link_libraries(coreTest PUBLIC 
                      core
                      utest
)
