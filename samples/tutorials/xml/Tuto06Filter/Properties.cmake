set( NAME Tuto06Filter )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations

    guiQt                   # Start the module, load qt implementation of gui

    # Objects declaration
    fwData
    servicesReg             # fwService

    # UI declaration/Actions
    gui
    style

    # Reader
    ioVTK

    # Services
    uiIO
    visuBasic
    opImageFilter # module containing the action to performs a threshold
)

moduleParam(guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via guiQt

moduleParam(
        appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto06Filter_AppCfg
) # Main application's configuration to launch
