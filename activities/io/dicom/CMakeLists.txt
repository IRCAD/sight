sight_add_target(activity_io_dicom TYPE MODULE)

add_dependencies(
    activity_io_dicom
    data
    io_dimse
    module_activity
    module_memory
    module_data
    module_ui
    module_ui_qt
    module_ui_dicom
    module_io_dicom
    module_io_dimse
    module_ui_icons
    module_service
    module_viz_scene3d
    module_viz_scene3d_qt
)
