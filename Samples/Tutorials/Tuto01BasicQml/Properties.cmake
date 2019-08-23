set( NAME Tuto01BasicQml ) # Name of the application
set( VERSION 0.1 ) # Version of the application
set( TYPE APP ) # Type APP represent "Application"
set( START ON) # This bundle will be automatically started
set( DEPENDENCIES
    fwRuntime
    fwData
    fwServices
    fwQml
)
set( REQUIREMENTS
    dataReg
    servicesReg
    guiQml
    fwlauncher
)
set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
)
