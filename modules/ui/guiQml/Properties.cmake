
set( NAME guiQml )
set( VERSION 0.1 )
set( TYPE MODULE )
set( START ON )
set( DEPENDENCIES
    core
    fwQml
    guiQt
    fwGuiQml
    services
)
set( REQUIREMENTS
    module_gui
)
set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
    platforms
    imageformats
)
