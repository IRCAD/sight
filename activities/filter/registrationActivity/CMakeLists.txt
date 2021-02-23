sight_target( registrationActivity TYPE MODULE )



add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
                 module_activity
                 module_io_matrix
                 module_filter_point
                 module_ui_base
                 module_ui_qt
                 module_data
                 module_viz_scene3d
                 module_viz_scene2d
                 module_ui_icons
                 module_geometry_base
                 module_viz_scene3dQt
                 config_viz_scene2d
                 module_filter_image
)
