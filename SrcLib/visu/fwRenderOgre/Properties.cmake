
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

set( CONAN_DEPS
    boost/1.67.0@sight/stable
    ogre/1.11.4@sight/stable
)