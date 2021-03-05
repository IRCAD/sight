sight_add_target( NetworkProxy TYPE EXECUTABLE CONSOLE ON )


target_link_libraries(NetworkProxy PUBLIC 
                      core
                      data
                      io_igtl
)
