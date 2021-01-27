set( NAME ExImageMix )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    visuVTK                 # Start the module, allow to use fwRenderVTK
    module_guiQt                   # Start the module, allow dark theme
    visuVTKQt               # Enable VTK to render things in Qt window

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_gui
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

moduleParam(module_guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_guiQt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExImageMix_AppCfg
) # Main application's configuration to launch
