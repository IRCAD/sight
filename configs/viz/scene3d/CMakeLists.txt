sight_add_target(config_viz_scene3d TYPE MODULE)

add_dependencies(
    config_viz_scene3d
    config_viz_image
    config_viz_scene2d
    data
    module_data
    module_filter_image
    module_service
    module_io_bitmap
    module_ui_base
    module_ui_qt
    module_ui_viz
    module_viz_scene2d
    module_viz_scene3d
    viz_scene2d
)
