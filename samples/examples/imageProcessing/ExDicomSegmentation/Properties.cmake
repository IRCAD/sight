set( NAME ExDicomSegmentation )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_io_base             # Start the module, load file location or window module_io_base
    visuOgre                # Start the module, allow to use fwRenderOgre
    module_ui_qt                   # Start the module, allow dark theme
    visuOgreQt              # Enable Ogre to render things in Qt window

    # Extensions
    activities

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_ui_base
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

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExDicomSegmentation_AppCfg
) # Main application's configuration to launch

