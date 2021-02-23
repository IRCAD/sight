sight_target( module_viz_sample TYPE MODULE WARNINGS_AS_ERRORS ON )


add_dependencies(${FWPROJECT_NAME} 
                 module_viz_scene3d
                 module_viz_scene3dQt
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      module_ui_qt
)
