set( NAME ExOgreRGBDStream )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_ui_base             # Start the module, load file location or window module_ui_base
    module_viz_scene3d                # Start the module, allow to use viz_scene3d
    module_ui_qt                   # Start the module, allow dark theme
    module_viz_scene3dQt              # Enable Ogre to render things in Qt window
    module_viz_scene2d                 # Start the module, allow to use viz_scene2d

    # Objects declaration
    data
    module_service             # fwService

    # UI declaration/Actions
    module_ui_base
    module_ui_flaticons

    # Grabber
    module_io_video    

    # Services
    module_sync

    viz_scene2d
)

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        module_ui_qt-0.1/flatdark.rcc
        module_ui_qt-0.1/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExOgreRGBDStream_AppCfg
) # Main application's configuration to launch
