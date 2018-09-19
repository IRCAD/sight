set( NAME TutoGuiQml ) # Name of the application
set( VERSION 0.1 ) # Version of the application
set( TYPE APP ) # Type APP represent "Application"
set( DEPENDENCIES  ) # For an application we have no dependencies (libraries to link)
set( REQUIREMENTS # The bundles used by this application
    dataReg
    servicesReg
    gui
    guiQt
    ioVTK
    uiIO # contains services to show dialogs for reader/writer selection
    visuVTKQt
    vtkSimpleNegato
    fwlauncher
    fwGuiQt
    appQml
    TestQML
    SBoolTest
    )

# Set the configuration to use : 'tutoBasicConfig'
bundleParam(appQml PARAM_LIST config PARAM_VALUES tutoBasicConfig)
