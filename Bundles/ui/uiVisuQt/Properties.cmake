
set( NAME uiVisuQt )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCom
    fwCore
    fwData
    fwDataTools
    fwGui
    fwGuiQt
    fwMath
    fwRuntime
    fwServices
    fwTools
)
set( REQUIREMENTS  )

set( CONAN_DEPS
    boost/1.67.0@fw4spl/stable
    qt/5.11.1@fw4spl/stable
    glm/0.9.8.5@fw4spl/stable
)