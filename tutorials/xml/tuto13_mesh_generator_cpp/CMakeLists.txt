sight_add_target(tuto13_mesh_generator_cpp TYPE APP UNIQUE ON)

add_dependencies(
    tuto13_mesh_generator_cpp
    sightrun
    data
    module_app
    module_ui
    module_ui_qt
    module_viz_scene3d
    module_viz_scene3d_qt
    module_service
    module_io_matrix
    module_io_vtk
)

target_link_libraries(tuto13_mesh_generator_cpp PUBLIC data geometry_data service ui)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES tuto13_mesh_generator_cpp_AppCfg)
