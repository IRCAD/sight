set( NAME Tuto11LaunchBasicConfig )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    guiQt                   # Start the module, load qt implementation of gui

    # Objects declaration
    fwData
    servicesReg             # fwService

    # UI declaration/Actions
    gui
    style

    # Reader
    ioData
    ioVTK

    # Services
    uiIO
    visuOgreBasic
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
    Tuto11LaunchBasicConfig_AppCfg
) # Main application's configuration to launch
