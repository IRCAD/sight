
set( NAME ioVTK )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCore
    fwData
    fwDataIO
    fwGui
    fwMedData
    fwMedDataTools
    fwRuntime
    fwServices
    fwTools
    fwVtkIO
    fwCom
    fwThread
    fwJobs
    fwIO
)
set( REQUIREMENTS  )

set( CONAN_DEPS
    vtk/8.0.1@sight/stable
)