
set( NAME guiQml )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( START ON )
set( DEPENDENCIES
    fwActivities
    fwCom
    fwCore
    fwData
    fwDataCamp
    fwDataTools
    fwMedData
    fwGui
    fwQml
    fwQt
    fwGuiQml
    fwRuntime
    fwServices
    fwTools
)
set( REQUIREMENTS
    gui
)
set( PLUGINS
    platforms
    imageformats
    styles
)
set( CONAN_DEPS
    ${CONAN_BOOST}
    ${CONAN_QT}
)
