sight_add_target( geometry_eigenTest TYPE TEST )


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      data
                      geometry_eigen
)
