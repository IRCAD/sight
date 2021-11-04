sight_add_target(ExSParameters TYPE APP)

add_dependencies(
    ExSParameters
    sightrun
    module_appXml
    module_ui_base
    module_ui_qt
    module_service
    module_ui_base
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_appXml PARAM_LIST config PARAM_VALUES ExSParameters_AppCfg)

sight_generate_profile(ExSParameters)
