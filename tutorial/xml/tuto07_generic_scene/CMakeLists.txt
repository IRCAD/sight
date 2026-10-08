sight_add_target(tuto07_generic_scene TYPE APP UNIQUE ON)

add_dependencies(
    tuto07_generic_scene
    data
    module_app
    module_io_bitmap # Allows to export a PNG
    module_io_document # Allows to export a PDF
    module_io_matrix # Allows reading/writing matrices from/to files
    module_io_vtk
    module_service
    module_ui # Provides the user interface framework
    module_ui_icons # Resource-only module that provides icons
    module_ui_qt
    module_ui_viz # Widgets dedicated to visualisation
    module_viz_scene3d # Used to render a 3D scene
    module_viz_scene3d_qt # Enable 3D rendering in a Qt widget
    sightrun
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES tuto07_generic_scene)
