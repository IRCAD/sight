sight_add_target(ex_notifications TYPE APP)

add_dependencies(
    ex_notifications
    sightrun
    module_app
    module_ui
    module_ui_qt
    module_service
    module_ui
)

target_link_libraries(ex_notifications PUBLIC ui)

# Allow dark theme via module_ui_qt
module_param(
    module_ui_qt PARAM_LIST resource stylesheet PARAM_VALUES sight::module::ui::qt/flatdark.rcc
                                                             sight::module::ui::qt/flatdark.qss
)

# Main application's configuration to launch
module_param(module_app PARAM_LIST config PARAM_VALUES ex_notifications_AppCfg)

sight_generate_profile(ex_notifications)
