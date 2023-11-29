sight_add_target(tuto14_gui TYPE APP)

add_dependencies(
    tuto14_gui
    sightrun
    module_app
    module_ui
    module_ui_qt
    data
    module_service
    module_ui
    module_ui_flaticons
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES Tuto14Gui_AppCfg)

sight_generate_profile(tuto14_gui)
