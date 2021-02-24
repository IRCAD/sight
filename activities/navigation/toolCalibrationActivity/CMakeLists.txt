sight_add_target( toolCalibrationActivity TYPE MODULE )


add_dependencies(${FWPROJECT_NAME} 
                 module_ui_base
                 module_ui_qt
                 module_data
                 module_viz_scene3d
                 module_ui_icons
                 module_io_video
                 module_viz_scene3dQt
                 data
                 module_service
                 module_navigation_calibration
                 module_geometry_vision
                 module_io_igtl
                 module_io_matrix
                 module_sync
)
