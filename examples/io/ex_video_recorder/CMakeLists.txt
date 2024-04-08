sight_add_target(ex_video_recorder TYPE APP UNIQUE ON)

add_dependencies(
    ex_video_recorder
    sightrun
    module_app
    module_ui
    module_viz_scene3d
    module_ui_qt
    module_viz_scene3d_qt
    data
    module_service
    module_ui
    module_ui_icons
    module_sync
    module_io_video
    config_io_video
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ExVideoRecorder_AppCfg)

sight_generate_profile(ex_video_recorder)
