sight_add_target( io_baseTest TYPE TEST )


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      data
                      io_base
)
