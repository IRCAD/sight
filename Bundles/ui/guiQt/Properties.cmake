
set( NAME guiQt )
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
    fwGuiQt
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
)
set( CONAN_DEPS
    boost/1.67.0@sight/stable
    qt/5.11.2@sight/stable
)
