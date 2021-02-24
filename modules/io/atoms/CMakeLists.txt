sight_add_target( module_io_atoms TYPE MODULE )





add_dependencies(${FWPROJECT_NAME} 
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      atoms
                      io_atoms
                      io_patch
                      core
                      data
                      ui_base
                      service
                      io_zip
                      io_base
)
