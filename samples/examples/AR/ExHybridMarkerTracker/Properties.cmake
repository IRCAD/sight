set(NAME ExHybridMarkerTracker)
set(VERSION 0.2)
set(TYPE APP)
set(DEPENDENCIES)
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    visuOgre                # Start the module, allow to use fwRenderOgre
    material                # Start the module, load Ogre's materials
    guiQt                   # Start the module, allow dark theme
    visuOgreQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    fwData
    servicesReg             # fwService
    arData

    # UI declaration/Actions
    gui
    style
    flatIcon

    # Grabber
    videoQt
    videoOpenCV

    # Services
    videoTools
    uiTools
    videoCalibration
    ctrlCamp
    ctrlCom
    maths
    ioCalibration
    hybridMarkerTracker

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
        ExHybridMarkerTracker_AppCfg
) # Main application's configuration to launch

