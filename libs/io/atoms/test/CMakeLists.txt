sight_add_target( io_atomsTest TYPE TEST)

target_link_libraries(io_atomsTest PUBLIC
                      core
                      io_atoms
                      atoms
                      io_zip
                      utestData
)
