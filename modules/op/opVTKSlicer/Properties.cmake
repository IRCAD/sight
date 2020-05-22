set( NAME opVTKSlicer )
set( VERSION 0.1 )
set( TYPE MODULE )
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
    ${CONAN_VTK}
)