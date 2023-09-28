sight_add_target(ExSParameters TYPE APP)

add_dependencies(
    ExSParameters
    sightrun
    module_appXml
    module_ui
    module_ui_qt
    module_service
    module_ui
)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet touch_friendly PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                                            sight::module::ui::qt/flatdark.qss true
)

# Main application's configuration to launch
module_param(module_appXml PARAM_LIST config PARAM_VALUES ExSParameters_AppCfg)

sight_generate_profile(ExSParameters)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif()
