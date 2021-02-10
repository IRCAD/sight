
set( NAME module_io_dicom )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    core
    data
    io_base
    io_dicom
    ui_base
    services
    )
set( REQUIREMENTS
    data
    module_ui_dicom
    module_services
    )
