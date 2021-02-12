set( NAME Tuto01BasicQml ) # Name of the application
set( VERSION 0.2 ) # Version of the application
set( TYPE APP ) # Type APP represent "Application"
set( START ON) # This module will be automatically started
set( DEPENDENCIES
    data
    services
    ui_qml
)
set( REQUIREMENTS
    data
    module_services
    module_ui_qml
    fwlauncher
)
set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
