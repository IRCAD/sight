set( NAME Tuto01BasicQml ) # Name of the application
set( VERSION 0.2 ) # Version of the application
set( TYPE APP ) # Type APP represent "Application"
set( START ON) # This module will be automatically started
set( DEPENDENCIES
    fwRuntime
    fwData
    fwServices
    fwQml
)
set( REQUIREMENTS
    fwData
    servicesReg
    guiQml
    fwlauncher
)
set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
