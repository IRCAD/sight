sight_target( io_patchTest TYPE TEST )





target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      io_patch
                      io_atoms
                      atoms
)
