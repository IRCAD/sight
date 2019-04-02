
set( NAME uiNetwork )
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
    ioNetwork
    uiIO
)
set( REQUIREMENTS  )
set( CONAN_DEPS
    ${CONAN_BOOST}
    ${CONAN_QT}
)