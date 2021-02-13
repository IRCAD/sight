
set( NAME module_filter_image )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
    data
    service
    filter_image
    io_vtk          # Necessary only for SPlaneSlicer
    ui_base
    ui_history
)
set( REQUIREMENTS )
