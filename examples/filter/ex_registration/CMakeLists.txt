sight_add_target(ex_registration TYPE APP)

add_dependencies(
    ex_registration
    sightrun
    module_app
    module_ui
    module_viz_scene3d
    module_ui_qt
    module_viz_scene3d_qt
    data
    module_service
    module_ui
    module_io_vtk
    module_io_matrix
    module_filter_point
    module_data
    module_geometry
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ExRegistration_AppCfg)
