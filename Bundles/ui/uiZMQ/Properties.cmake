
set( NAME uiZMQ )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCore
    fwTools
    fwData
    fwServices
    fwCom
    fwRuntime
    fwGui
    fwGuiQt
    zmqNetwork
    
    ioZMQ
)
set( REQUIREMENTS)
set( CONAN_DEPS
    ${CONAN_QT}
)