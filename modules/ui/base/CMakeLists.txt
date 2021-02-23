fwLoadProperties()

add_dependencies(${FWPROJECT_NAME} 
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      atoms
                      core
                      data
                      service
                      activity
                      io_base
                      io_atoms
                      ui_base
)
