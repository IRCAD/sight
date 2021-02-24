sight_add_target( module_ui_console TYPE MODULE START ON )


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      ui_base
                      service
)
