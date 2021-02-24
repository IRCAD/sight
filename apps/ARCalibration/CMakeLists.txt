sight_add_target( ARCalibration TYPE APP )


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

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight_sight_module_ui_qt/flatdark.rcc
        sight_sight_module_ui_qt/flatdark.qss
) # Allow dark theme via module_ui_qt
moduleParam(
        module_appXml
    PARAM_LIST
        config parameters
    PARAM_VALUES
        ARCalibration_AppCfg ARCalibration_AppCfgParam
) # Main application's configuration to launch
