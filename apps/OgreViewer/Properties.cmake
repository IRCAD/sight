set( NAME OgreViewer )
set( VERSION 0.5 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    visuOgre                # Start the module, allow to use fwRenderOgre
    guiQt                   # Start the module, allow dark theme
    patchMedicalData        # Patch old atoms data

    visuOgreQt              # Enable Ogre to render things in Qt window.

    # Objects declaration
    servicesReg
    data
    ioPacs

    # UI declaration/Actions
    gui
    style
    uiVisuOgre
    uiIO
    uiImageQt
    uiTF
    flatIcon
    uiMeasurementQt

    # Reader
    ioVTK
    ioAtoms
    ioGdcm

    # Services
    scene2D
    ctrlCamp
    ctrlTF
    uiMeasurement
    opImageFilter
    ioPacs
    uiMedDataQt
    ctrlSelection

    # Configuration launchers
    ogreConfig

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
        OgreViewer_AppCfg
) # Main application's configuration to launch
