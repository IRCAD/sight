sight_add_target(tuto11_picker TYPE APP UNIQUE ON)

add_dependencies(
    tuto11_picker
    sightrun
    module_app
    module_ui
    module_ui_qt
    module_viz_scene3d
    module_viz_scene3d_qt
    data
    module_service
    module_ui
    module_ui_icons
    module_io_matrix
    module_io_vtk
    module_io_bitmap
    module_data
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)
module_param(module_app PARAM_LIST config PARAM_VALUES Tuto11Picker_AppCfg)
