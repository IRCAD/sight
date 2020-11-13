set( NAME ExRGBDManualAR )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
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
    fwMedData
    servicesReg             # fwService
    arData

    # UI declaration/Actions
    gui
    style
    flatIcon

    # Reader
    ioData
    ioVTK
    ioCalibration

    # Grabber
    videoQt

    # Services
    videoTools
    uiIO
    ctrlCamp
    maths
    uiPreferences
    colourSegmentation

    # Configuration
    dataManagerConfig

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
        ExRGBDManualAR_AppCfg
) # Main application's configuration to launch
