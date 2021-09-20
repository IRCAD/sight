sight_add_target( geometry_glmTest TYPE TEST)

target_link_libraries(geometry_glmTest PUBLIC 
                      core
                      utestData
                      geometry_glm
)
