sight_add_target(ExVideoRecorder TYPE APP UNIQUE ON)

add_dependencies(
    ExVideoRecorder
    sightrun
    module_app
    module_ui
    module_viz_scene3d
    module_ui_qt
    module_viz_scene3dQt
    data
    module_service
    module_ui
    module_ui_flaticons
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

sight_generate_profile(ExVideoRecorder)
