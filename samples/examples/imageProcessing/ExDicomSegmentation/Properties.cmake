set( NAME ExDicomSegmentation )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    visuOgre                # Start the module, allow to use fwRenderOgre
    guiQt                   # Start the module, allow dark theme
    material                # Start the module, load Ogre's materials
    visuOgreQt              # Enable Ogre to render things in Qt window

    # Extensions
    activities

    # Objects declaration
    fwData
    servicesReg             # fwService
    fwMedData

    # UI declaration/Actions
    gui
    style
    flatIcon

    # Reader
    ioActivity
    ioData
    ioGdcm
    ioVTK
    ioAtoms

    # Config
    3DVisualizationActivity
    2DVisualizationActivity

    # Services
    opImageFilter
    ctrlCamp
    opVTKMesh
    uiMedDataQt
    ctrlSelection
    uiIO
    uiActivitiesQt
    uiReconstructionQt
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
        ExDicomSegmentation_AppCfg
) # Main application's configuration to launch

