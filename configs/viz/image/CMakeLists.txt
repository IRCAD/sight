sight_add_target(config_viz_image TYPE MODULE)

add_dependencies(
    config_viz_image
    activity_viz_negato
    config_viz_scene2d
    data
    module_service
    module_ui
    module_ui_qt
    module_data
    module_viz_scene3d
    module_viz_scene3d_qt
    module_viz_scene2d
    module_ui_icons
)
