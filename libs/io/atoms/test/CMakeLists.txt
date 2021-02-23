sight_target( io_atomsTest TYPE TEST )


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      io_atoms
                      atoms
                      io_zip
                      utestData
)
