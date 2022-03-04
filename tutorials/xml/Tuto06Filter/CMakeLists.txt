sight_add_target(Tuto06Filter TYPE APP)

add_dependencies(
    Tuto06Filter
    sightrun
    module_appXml
    module_ui_qt
    data
    module_service
    module_ui_base
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
module_param(module_appXml PARAM_LIST config PARAM_VALUES Tuto06Filter_AppCfg)

sight_generate_profile(Tuto06Filter)
