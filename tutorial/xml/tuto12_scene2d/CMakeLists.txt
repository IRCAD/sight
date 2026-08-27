sight_add_target(tuto12_scene2d TYPE APP)

add_dependencies(
    tuto12_scene2d
    module_app
    module_service
    module_ui
    module_ui_qt
    module_viz_scene2d # 2D visualization module
    sightrun
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Allow dark theme via module_ui_qt
module_param(module_app PARAM_LIST config PARAM_VALUES Tuto12Scene2D_AppCfg)
