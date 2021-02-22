set( NAME Tuto17SimpleAR )
set( VERSION 0.5 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher                     # Needed to build the launcher
    module_appXml                  # XML configurations

    module_ui_base                 # Start the module, load file location or window module_ui_base
    module_ui_qt                   # Start the module, load module_ui_qt implementation of module_ui_base
    module_viz_scene3d                # Start the module, allow to use viz_scene3d
    module_viz_scene3dQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    data
    module_service

    # UI declaration/Actions
    module_ui_base
    module_ui_flaticons

    # Reader
    module_io_matrix
    module_io_vtk
    module_io_atoms

    # Grabber
    module_io_video

    # Services
    module_io_vision
    module_data
    module_sync
    module_navigation_optics
    module_geometry_vision
    module_geometry_vision
    module_geometry_base
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
        Tuto17SimpleAR_AppCfg
) # Main application's configuration to launch
