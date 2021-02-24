sight_add_target( coreTest TYPE TEST WARNINGS_AS_ERRORS ON )


if(UNIX)
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE CAMP_LIB_DIR="/lib/x86_64-linux-gnu")
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE MODULE_LIB_PREFIX="${SIGHT_MODULE_LIB_PREFIX}")
    target_compile_definitions(${FWPROJECT_NAME} PRIVATE MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")
    target_link_libraries(${FWPROJECT_NAME} PUBLIC "dl")
endif()

add_dependencies(${FWPROJECT_NAME} 
                 module_utest
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utest
)
