
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
    ${CONAN_BOOST}
    ${CONAN_QT}
    ${CONAN_GLM}
)