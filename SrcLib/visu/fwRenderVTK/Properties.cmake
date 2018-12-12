
set( NAME fwRenderVTK )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCom
    fwCore
    fwData
    fwGui
    fwRender
    fwRuntime
    fwServices
    fwTools
    fwVtkIO
)

set( REQUIREMENTS  )
set( CONAN_DEPS
    vtk/8.0.1-r1@sight/stable
)