sight_add_target(ExStereoARCV TYPE APP)

add_dependencies(
    ExStereoARCV
    sightrun
    module_app
    module_ui
    module_viz_scene3d
    module_ui_qt
    module_viz_scene3d_qt
    data
    module_service
    module_ui
    module_ui_flaticons
    module_io_vision
    module_io_video
    module_sync
    module_geometry_vision
    module_geometry
    module_geometry_vision
    module_navigation_optics
    module_io_matrix
    module_data
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ExStereoARCV_AppCfg)

sight_generate_profile(ExStereoARCV)
