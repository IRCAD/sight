sight_add_target( SightCalibrator TYPE APP )

add_dependencies(${FWPROJECT_NAME}
                 sightrun
                 module_appXml
                 module_ui_base
                 module_ui_qt
                 module_service
                 module_sync
                 data
                 module_ui_base
                 module_ui_flaticons
                 module_ui_icons
                 module_io_atoms
                 module_io_vision
                 module_io_video
                 module_io_activity
                 module_io_realsense
                 module_io_igtl
                 module_activity
                 activity_navigation_calibration
                 module_data
                 module_memory
                 module_geometry_vision
                 module_geometry_base
                 module_navigation_calibration
)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight::module::ui::qt/flatdark.rcc
        sight::module::ui::qt/flatdark.qss
) # Allow dark theme via module_ui_qt
moduleParam(
        module_appXml
    PARAM_LIST
        config parameters
    PARAM_VALUES
        SightCalibrator_AppCfg SightCalibrator_AppCfgParam
) # Main application's configuration to launch

sight_generate_profile(${FWPROJECT_NAME})
