
set( NAME guiQml )
set( VERSION 0.1 )
set( TYPE MODULE )
set( START ON )
set( DEPENDENCIES
    core
    fwGui
    fwQml
    fwQt
    fwGuiQml
    services
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