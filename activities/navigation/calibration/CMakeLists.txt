sight_add_target(activity_navigation_calibration TYPE MODULE)

add_dependencies(
    activity_navigation_calibration
    module_ui
    module_viz_scene3d
    module_viz_scene3dQt
    module_activity
    module_service
    module_sync
    data
    module_ui_icons
    module_ui_qt
    module_io_matrix
    module_io_vision
    module_io_video
    module_io_igtl
    module_geometry_vision
    module_geometry
    module_sync
    module_data
    module_navigation_calibration
)
