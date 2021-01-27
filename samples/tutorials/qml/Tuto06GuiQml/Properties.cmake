set( NAME Tuto06GuiQml ) # Name of the application
set( VERSION 0.2 ) # Version of the application
set( TYPE APP ) # Type APP represent "Application"
set( START ON)
set( DEPENDENCIES
    data
    services
    fwQml
)
set( REQUIREMENTS # The modules used by this application
    data
    module_services
    module_gui
    module_guiQt
    )
set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
