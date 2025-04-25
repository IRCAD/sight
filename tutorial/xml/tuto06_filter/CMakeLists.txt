sight_add_target(tuto06_filter TYPE APP)

add_dependencies(
    tuto06_filter
    sightrun
    module_app
    module_ui_qt
    data
    module_service
    module_ui
    module_io_vtk
    module_viz_sample
    module_filter_image
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES Tuto06Filter_AppCfg)
