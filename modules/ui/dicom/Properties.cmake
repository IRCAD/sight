
set( NAME module_ui_dicom )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    core
    filter_dicom
    ui_dicom
    ui_base
    module_ui_qt
    service
    io_base
    data
    )
set( REQUIREMENTS
    data
    module_service
)
