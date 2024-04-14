sight_add_target(ex_frame_by_frame TYPE APP UNIQUE ON)

add_dependencies(
    ex_frame_by_frame
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
    module_io_video
    module_sync
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ExFrameByFrame_AppCfg)

sight_generate_profile(ex_frame_by_frame)
