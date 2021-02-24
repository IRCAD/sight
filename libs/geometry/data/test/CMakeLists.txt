sight_add_target( geometry_dataTest TYPE TEST )







target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      geometry_data
                      utestData
)
