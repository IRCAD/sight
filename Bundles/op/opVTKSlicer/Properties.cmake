set( NAME opVTKSlicer )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCom
    fwCore
    fwData
    fwDataTools
    fwRuntime
    fwServices
    fwVtkIO
)
set( CONAN_DEPS
    vtk/8.0.1@sight/stable
)