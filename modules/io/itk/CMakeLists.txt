sight_add_target( module_io_itk TYPE MODULE )




target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      ui_base
                      io_itk
                      service
                      io_base
)
