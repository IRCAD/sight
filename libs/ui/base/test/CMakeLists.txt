fwLoadProperties()

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
