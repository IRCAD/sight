sight_add_target(ex_parameters TYPE APP)

add_dependencies(ex_parameters sightrun module_app module_ui module_ui_qt module_service)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet touch_friendly PARAM_VALUES sight::module::ui::qt/noctura.rcc
                                                                            sight::module::ui::qt/noctura.qss true
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ex_parameters_app_cfg)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/uit)
endif()
