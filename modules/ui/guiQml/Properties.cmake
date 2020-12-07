
set( NAME guiQml )
set( VERSION 0.1 )
set( TYPE MODULE )
set( START ON )
set( DEPENDENCIES
    fwCore
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
    QtQml
    QtQuick
    QtQuick.2
    platforms
    imageformats
)