sight_add_target(ExRealSense TYPE APP UNIQUE ON)

add_dependencies(
    ExRealSense
    sightrun
    module_appXml
    module_ui_base
    module_viz_scene2d
    module_viz_scene3d
    module_ui_qt
    module_viz_scene3dQt
    data
    module_service
    module_ui_base
    module_ui_flaticons
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
module_param(module_appXml PARAM_LIST config PARAM_VALUES ExRealSense_AppCgf)

sight_generate_profile(ExRealSense)
