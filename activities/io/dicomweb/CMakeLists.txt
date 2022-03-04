sight_add_target(activity_io_dicomweb TYPE MODULE)

add_dependencies(
    activity_io_dicomweb
    module_activity
    module_memory
    module_data
    data
    module_ui_base
    module_ui_qt
    module_io_dicom
    module_ui_dicom
    module_io_dicomweb
    module_ui_icons
    module_service
    module_viz_scene3d
    module_viz_scene3dQt
)
