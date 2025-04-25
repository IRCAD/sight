sight_add_target(config_io_dimse TYPE MODULE)

add_dependencies(
    config_io_dimse
    config_io_dicomweb
    data
    io_dimse
    module_data
    module_memory
    module_io_dicom
    module_activity
    module_ui_dicom
    module_io_dimse
    module_io_dicomweb
    module_service
    module_ui_icons
    module_ui
    module_ui_qt
    module_viz_scene3d
    module_viz_scene3d_qt
)
