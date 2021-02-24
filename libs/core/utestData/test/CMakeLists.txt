sight_add_target( utestDataTest TYPE TEST )

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      data
)
