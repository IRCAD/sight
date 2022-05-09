sight_add_target(config_viz_scene2d TYPE MODULE)

add_dependencies(
    config_viz_scene2d
    data
    module_data
    module_service
    module_ui_base
    module_ui_qt
    module_viz_scene2d
    module_ui_icons
    viz_scene2d
)
