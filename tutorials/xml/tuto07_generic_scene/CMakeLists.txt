sight_add_target(tuto07_generic_scene TYPE APP UNIQUE ON)

add_dependencies(
    tuto07_generic_scene
    sightrun
    module_app
    module_ui
    module_ui_qt
    module_ui_viz
    module_viz_scene3d
    module_viz_scene3d_qt
    data
    module_service
    module_ui_icons
    module_io_matrix
    module_io_vtk
    module_io_bitmap
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES Tuto07GenericScene_AppCfg)
