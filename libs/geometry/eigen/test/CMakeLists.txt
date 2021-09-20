sight_add_target( geometry_eigenTest TYPE TEST)


target_link_libraries(geometry_eigenTest PUBLIC 
                      core
                      utestData
                      data
                      geometry_eigen
)
