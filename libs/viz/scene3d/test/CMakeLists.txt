sight_add_target( viz_scene3dTest TYPE TEST )

target_link_libraries(viz_scene3dTest PUBLIC
                      core
                      utestData
                      data
                      viz_scene3d
)

add_dependencies(viz_scene3dTest module_viz_scene3d_test)
