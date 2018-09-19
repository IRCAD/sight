set( NAME TutoGuiQml ) # Name of the application
set( VERSION 0.1 ) # Version of the application
set( TYPE APP ) # Type APP represent "Application"
set( START ON)
set( DEPENDENCIES
    fwRuntime
    fwData
    fwServices
    fwQml
)
set( REQUIREMENTS # The bundles used by this application
    dataReg
    servicesReg
    gui
    guiQt
    )
