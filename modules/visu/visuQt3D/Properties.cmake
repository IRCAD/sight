
set( NAME visuQt3D )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( PRIORITY 2)
set( DEPENDENCIES
    fwCore
    fwTools
    fwData
    fwDataTools
    fwGuiQt
    fwRender
    fwRenderQt3D
    fwRuntime
    fwServices
    fwGui
    fwCom
)
set( REQUIREMENTS  )

set( CONAN_DEPS
    ${CONAN_QT}
)