sight_add_target(ex_detect_cam_motion TYPE APP)

add_dependencies(
    ex_detect_cam_motion
    sightrun
    module_app
    module_ui
    module_viz_scene3d
    module_ui_qt
    module_viz_scene3d_qt
    data
    module_service
    module_ui_icons
    module_io_video
    module_filter_vision
    module_sync
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ExDetectCamMotion_AppCfg)

sight_generate_profile(ex_detect_cam_motion)
