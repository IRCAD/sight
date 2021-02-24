sight_add_target( filter_visionTest TYPE TEST )

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      filter_vision
                      utestData
)
