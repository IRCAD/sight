set( NAME TutoSimpleAR )
set( VERSION 0.5 )
set( TYPE APP )
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
    arData
    servicesReg             # fwService

    # UI declaration/Actions
    gui
    style
    flatIcon

    # Reader
    ioData
    ioVTK
    ioAtoms

    # Grabber
    videoQt

    # Services
    ioCalibration
    uiTools
    ctrlCamp
    videoTools
    trackerAruco
    registrationCV
    videoCalibration
    maths
    ctrlCom

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
        TutoSimpleAR_AppCfg
) # Main application's configuration to launch
