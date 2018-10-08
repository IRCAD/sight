
set( NAME opVTKMesh )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES 
    fwMedData
    fwCom
    fwData
    fwGui
    fwRuntime
    fwServices
    fwTools
    fwVtkIO
    fwCore
)
set( REQUIREMENTS  )

set( CONAN_DEPS
    vtk/8.0.1@sight/stable
)