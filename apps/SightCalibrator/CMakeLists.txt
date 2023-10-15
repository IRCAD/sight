sight_add_target(SightCalibrator TYPE APP)

add_dependencies(
    SightCalibrator
    sightrun
    module_app
    module_ui
    module_ui_qt
    module_service
    module_sync
    data
    module_ui
    module_ui_flaticons
    module_ui_icons
    module_io_vision
    module_io_video
    module_io_activity
    module_io_igtl
    module_io_session
    module_activity
    activity_navigation_calibration
    module_data
    module_memory
    module_geometry_vision
    module_geometry
    module_navigation_calibration
)

if(SIGHT_ENABLE_REALSENSE)
    add_dependencies(SightCalibrator module_io_realsense)
endif()

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)
# Main application's configuration to launch
module_param(module_app PARAM_LIST config parameters PARAM_VALUES SightCalibrator_AppCfg SightCalibrator_AppCfgParam)

sight_generate_profile(SightCalibrator)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif()
