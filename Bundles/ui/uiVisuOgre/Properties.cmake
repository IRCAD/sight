
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
        fwIO
        uiIO
)

set( REQUIREMENTS
        material
)

set( CONAN_DEPS
    ${CONAN_QT}
)