set( NAME fwRenderOgre )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    arData
    fwCom
    fwCore
    fwData
    fwDataTools
    fwGui
    fwMath
    fwRender
    fwRuntime
    fwServices
    fwTools
    fwThread
)
set( REQUIREMENTS  )
set( WARNINGS_AS_ERRORS ON )

set( CONAN_DEPS
    ${CONAN_OGRE}
)
