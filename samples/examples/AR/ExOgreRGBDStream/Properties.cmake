set( NAME ExOgreRGBDStream )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    visuOgre                # Start the module, allow to use fwRenderOgre
    guiQt                   # Start the module, allow dark theme
    visuOgreQt              # Enable Ogre to render things in Qt window
    scene2D                 # Start the module, allow to use fwRenderQt

    # Objects declaration
    data
    servicesReg             # fwService
    arData

    # UI declaration/Actions
    gui
    style
    flatIcon

    # Grabber
    videoQt
    videoOpenni
    videoOpenCV

    # Services
    videoTools

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
        ExOgreRGBDStream_AppCfg
) # Main application's configuration to launch
