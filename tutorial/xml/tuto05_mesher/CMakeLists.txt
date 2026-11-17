sight_add_target(tuto05_mesher TYPE APP)

add_dependencies(
    tuto05_mesher
    data
    module_app
    module_data # Provides services that performs generic actions on data such as copying or extracting objects.
    module_filter_mesh # Provides services to manipulates meshes, here to generate a mesh from an image.
    module_io_vtk
    module_service
    module_ui
    module_ui_qt
    module_viz_sample
    sightrun
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES tuto05_mesher)
