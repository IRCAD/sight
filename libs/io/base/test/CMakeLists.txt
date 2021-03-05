sight_add_target( io_baseTest TYPE TEST )


target_link_libraries(io_baseTest PUBLIC 
                      core
                      utestData
                      data
                      io_base
)
