sight_add_target(config_ui_activity TYPE MODULE)

add_dependencies(
    config_ui_activity
    data
    module_activity
    module_data
    module_ui
    module_ui_qt
    module_io_session
    module_io_activity
    module_ui_icons
    module_ui_icons
)
