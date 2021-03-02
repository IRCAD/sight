sight_add_target( module_viz_sample TYPE MODULE WARNINGS_AS_ERRORS ON )


add_dependencies(${FWPROJECT_NAME} 
                 module_viz_scene3d
                 module_viz_scene3dQt
                 module_ui_qt
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC ui_qt)
