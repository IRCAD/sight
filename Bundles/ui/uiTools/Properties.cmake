
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
    ${CONAN_BOOST}
    ${CONAN_QT}
)