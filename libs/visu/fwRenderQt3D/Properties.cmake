
set( NAME fwRenderQt3D )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCom
    fwData
    fwDataTools
    fwGuiQt
    fwRender
    fwRuntime
    fwServices
    ioVTK
)
set( REQUIREMENTS )

set( WARNINGS_AS_ERRORS ON )

set( CONAN_DEPS
    ${CONAN_QT}
)
