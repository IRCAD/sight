
set( NAME visuVTKQt )
set( VERSION 0.1 )
set( TYPE BUNDLE )
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
)
set( REQUIREMENTS
    visuVTK
)
set( USE_PCH_FROM_TARGET fwRenderVTK )
set( START_BEFORE guiQt )
set( CONAN_DEPS
    qt/5.11.1@fw4spl/stable
)