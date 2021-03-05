sight_add_target( module_io_igtl TYPE MODULE )




target_link_libraries(module_io_igtl PUBLIC 
                      core
                      data
                      service
                      ui_base
                      io_base
                      io_igtl
)
