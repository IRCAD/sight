sight_add_target(config_io_video TYPE MODULE)

add_dependencies(
    config_io_video
    data
    module_data
    module_service
    module_io_video
    module_ui_icons
    module_ui
    module_ui_qt
)
