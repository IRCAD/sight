sight_add_target(Tuto12Scene2D TYPE APP)

add_dependencies(
    Tuto12Scene2D
    sightrun
    module_app
    module_ui_qt
    module_service
    module_ui
    module_viz_scene2d
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Allow dark theme via module_ui_qt
module_param(module_app PARAM_LIST config PARAM_VALUES Tuto12Scene2D_AppCfg)

sight_generate_profile(Tuto12Scene2D)
