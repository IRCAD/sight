sight_add_target(Tuto16SceneQt3D TYPE APP UNIQUE ON)

add_dependencies(
    Tuto16SceneQt3D
    sightrun
    module_app
    module_ui_qt
    data
    module_service
    module_ui
    module_io_vtk
    module_io_session
    module_viz_qt3d
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES Tuto16SceneQt3D_AppCfg)

sight_generate_profile(Tuto16SceneQt3D)
