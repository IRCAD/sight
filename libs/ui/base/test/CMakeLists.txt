sight_target( ui_base_test TYPE TEST WARNINGS_AS_ERRORS ON )


add_dependencies(${FWPROJECT_NAME} 
                 module_service
                 module_ui_base
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      ui_base
                      utest
)
