sight_add_target( io_patchTest TYPE TEST WARNINGS_AS_ERRORS OFF)

target_link_libraries(io_patchTest PUBLIC
                      core
                      utestData
                      io_patch
                      io_atoms
                      atoms
)
