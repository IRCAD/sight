
set( NAME uiVisuOgre )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
        fwCore
        fwData
        fwCom
        fwRuntime
        fwServices
        fwTools
        fwGui
        fwGuiQt
        fwRenderOgre
        uiIO
        material
)

set( CONAN_DEPS
    qt/5.11.1@sight/stable
)