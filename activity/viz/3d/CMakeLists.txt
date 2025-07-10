sight_add_target(activity_viz_3d TYPE MODULE)

add_dependencies(
    activity_viz_3d
    module_activity
    module_data
    module_ui
    module_ui_qt
    module_memory
    module_data
    module_service
    module_viz_scene3d_qt
    module_viz_scene3d
    module_ui_icons
    data
)
