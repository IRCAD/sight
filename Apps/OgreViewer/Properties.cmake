set( NAME OgreViewer )
set( VERSION 0.4 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    appXml                  # XML configurations

    preferences             # Start the bundle, load file location or window preferences
    visuOgre                # Start the bundle, allow to use fwRenderOgre
    material                # Start the bundle, load Ogre's materials
    guiQt                   # Start the bundle, allow dark theme
    patchMedicalData        # Patch old atoms data

    visuOgreQt              # Enable Ogre to render things in Qt window.

    # Objects declaration
    dataReg                 # fwData
    servicesReg             # fwService

    # UI declaration/Actions
    gui
    style
    uiVisuOgre
    uiIO
    uiImageQt
    uiTF
    flatIcon

    # Reader
    ioVtkGdcm
    ioVTK
    ioAtoms

    # Services
    scene2D
    ctrlCamp
    ctrlTF
    ctrlPicking
    uiMeasurement

    # Configuration launchers
    ogreConfig

    # Generic Scene
    visuOgreAdaptor
    )

bundleParam(guiQt
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
        OgreViewer_AppCfg
) # Main application's configuration to launch
