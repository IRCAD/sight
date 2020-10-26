set( NAME ExImageMix )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    visuVTK                 # Start the module, allow to use fwRenderVTK
    guiQt                   # Start the module, allow dark theme
    visuVTKQt               # Enable VTK to render things in Qt window

    # Objects declaration
    fwData
    servicesReg             # fwService

    # UI declaration/Actions
    gui
    style

    # Reader
    ioVTK
    ioData

    # Services
    uiVisuQt
    uiIO
    ctrlSelection
    uiImageQt

    # Generic Scene
    visuVTKAdaptor
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
        ExImageMix_AppCfg
) # Main application's configuration to launch
