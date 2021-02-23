sight_target( ARCalibration TYPE APP )


add_dependencies(${FWPROJECT_NAME} 
                 fwlauncher
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_service
                 data
                 module_ui_base
                 module_ui_flaticons
                 module_ui_icons
                 module_io_atoms
                 module_io_vision
                 module_io_activity
                 module_io_realsense
                 module_ui_base
                 module_activity
                 activity_navigation_calibration
                 toolCalibrationActivity
                 module_data
                 module_memory
)
