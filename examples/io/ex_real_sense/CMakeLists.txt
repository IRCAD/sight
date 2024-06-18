sight_add_target(ex_real_sense TYPE APP UNIQUE ON)

add_dependencies(
    ex_real_sense
    sightrun
    module_app
    module_ui
    module_viz_scene2d
    module_viz_scene3d
    module_ui_qt
    module_viz_scene3d_qt
    data
    module_service
    module_ui
    module_ui_icons
    module_io_video
    module_io_realsense
    module_sync
    module_data
    viz_scene2d
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ExRealSense_AppCgf)
