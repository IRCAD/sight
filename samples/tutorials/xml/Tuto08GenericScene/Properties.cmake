set( NAME Tuto08GenericScene )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    guiQt                   # Start the module, load qt implementation of gui
    visuOgre                # Start the module, allow to use fwRenderOgre
    material                # Start the module, load Ogre's materials
    visuOgreQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    fwData
    servicesReg             # fwService

    # UI declaration/Actions
    gui
    style
    flatIcon

    # Reader
    ioData
    ioVTK
    ioAtoms

    # Services
    uiIO
    uiImageQt

    # Generic Scene
    visuOgreAdaptor
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
        Tuto08GenericScene_AppCfg
) # Main application's configuration to launch
