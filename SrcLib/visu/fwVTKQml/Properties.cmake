
set( NAME fwVTKQml )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwData
    fwGui
    fwGuiQt
    fwRenderVTK
    fwRuntime
    fwServices
    fwTools
    fwCom
    fwQml
)

set( USE_PCH_FROM_TARGET fwRenderVTK )
