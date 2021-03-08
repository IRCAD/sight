sight_add_target( io_zipTest TYPE TEST )


target_link_libraries(io_zipTest PUBLIC 
                      utestData
                      io_zip
)
