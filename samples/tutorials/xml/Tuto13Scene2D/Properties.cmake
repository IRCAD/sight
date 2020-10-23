set( NAME Tuto13Scene2D )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    appXml                  # XML configurations

    guiQt                   # Start the module, load qt implementation of gui

    # Objects declaration
    servicesReg             # fwService

    # UI declaration/Actions
    gui
    style

    # Generic Scene
    scene2D
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
        Tuto13Scene2D_AppCfg
   ) # Allow dark theme via guiQt
