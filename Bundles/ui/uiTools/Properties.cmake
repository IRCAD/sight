
set( NAME uiTools )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES 
    fwRuntime
    fwCore
    fwTools
    fwData
    arData
    fwServices
    fwGui
    fwGuiQt
    fwCom
)
set( REQUIREMENTS )

set( CONAN_DEPS
    boost/1.67.0@fw4spl/stable
    qt/5.11.1@fw4spl/stable
)