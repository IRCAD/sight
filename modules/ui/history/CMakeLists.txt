sight_add_target( module_ui_history TYPE MODULE )


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      service
                      ui_history
)
