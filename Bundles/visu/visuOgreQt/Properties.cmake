
set( NAME visuOgreQt )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( START ON )
set( DEPENDENCIES
    fwCore
    fwCom
    fwData
    fwGui
    fwGuiQt
    fwRenderOgre
    fwRuntime
    fwServices
    fwTools
)
set( REQUIREMENTS visuOgre )
set( CONAN_DEPS
    ${CONAN_QT}
)
set( WARNINGS_AS_ERRORS ON )
