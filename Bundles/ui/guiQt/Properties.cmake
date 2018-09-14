
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
set( REQUIREMENTS  )
set( PLUGINS
    platforms
    imageformats
)
set( CONAN_DEPS
    boost/1.67.0@fw4spl/stable
    qt/5.11.1@fw4spl/stable
)