sight_add_target( dataTest TYPE TEST )

target_link_libraries(dataTest PUBLIC 
                      atoms
                      core
                      utestData
                      data
)
