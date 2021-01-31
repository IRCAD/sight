set( NAME ExImageMix )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_io_base             # Start the module, load file location or window module_io_base
    visuVTK                 # Start the module, allow to use fwRenderVTK
    module_ui_qt                   # Start the module, allow dark theme
    visuVTKQt               # Enable VTK to render things in Qt window

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_ui_base

    # Reader
    module_io_vtk
    module_io_file

    # Services
    uiIO
    ctrlSelection

    # Generic Scene
    visuVTKAdaptor
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
        ExImageMix_AppCfg
) # Main application's configuration to launch
