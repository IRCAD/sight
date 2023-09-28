sight_add_target(Tuto13MeshGeneratorCpp TYPE APP UNIQUE ON)

add_dependencies(
    Tuto13MeshGeneratorCpp
    sightrun
    data
    module_appXml
    module_ui
    module_ui_qt
    module_viz_scene3d
    module_viz_scene3dQt
    module_service
    module_io_matrix
    module_io_vtk
)

target_link_libraries(Tuto13MeshGeneratorCpp PUBLIC data geometry_data service ui)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_appXml PARAM_LIST config PARAM_VALUES Tuto13MeshGeneratorCpp_AppCfg)

sight_generate_profile(Tuto13MeshGeneratorCpp)
