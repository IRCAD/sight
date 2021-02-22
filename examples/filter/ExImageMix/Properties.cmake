set( NAME ExImageMix )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher                 # Needed to build the launcher
    module_appXml              # XML configurations

    module_ui_base             # Start the module, load file location or window module_ui_base
    module_ui_qt               # Start the module, allow dark theme

    # Objects declaration
    data
    module_service

    # UI declaration/Actions
    module_ui_base

    # Reader
    module_io_vtk
    module_io_matrix

    # Services
    module_data

    # Generic Scene
    module_viz_scene3d
    module_viz_scene3dQt            # Enable Ogre to render things in Qt window
)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        sight_module_ui_qt/flatdark.rcc
        sight_module_ui_qt/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExImageMix_AppCfg
) # Main application's configuration to launch
