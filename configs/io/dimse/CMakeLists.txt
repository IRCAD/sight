sight_add_target(config_io_dimse TYPE MODULE)

add_dependencies(
    config_io_dimse
    data
    io_dimse
    module_data
    module_io_dimse
    module_service
    module_ui_icons
    module_ui
    module_ui_qt
    module_viz_scene3d
)
