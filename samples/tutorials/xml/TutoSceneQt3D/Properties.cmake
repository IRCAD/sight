set( NAME TutoSceneQt3D )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    appXml                  # XML configurations

    guiQt                   # Start the module, allow dark theme

    # Objects declaration
    fwData
    servicesReg             # fwService

    # UI declaration/Actions
    gui
    style
    uiIO

    # Reader
    ioVTK
    ioAtoms

    # Generic Scene
    visuQt3DAdaptor
    visuQt3DQt
)

moduleParam(guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via guiQt

bundleParam(
        appXml
    PARAM_LIST
        config
    PARAM_VALUES
        TutoSceneQt3D_AppCfg
) # Main application's configuration to launch
