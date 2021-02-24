sight_add_target( io_zipTest TYPE TEST )


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      utestData
                      io_zip
)
