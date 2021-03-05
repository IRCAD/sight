sight_add_target( module_viz_scene3d TYPE MODULE START ON WARNINGS_AS_ERRORS ON )

target_link_libraries(module_viz_scene3d PUBLIC 
                      viz_scene3d
)
