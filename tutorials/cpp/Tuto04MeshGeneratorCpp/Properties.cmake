set( NAME Tuto04MeshGeneratorCpp )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES
    data
    geometry_data
    service
    ui_base
    )
set( REQUIREMENTS
    fwlauncher                     # Needed to build the launcher
    module_appXml                  # XML configurations

    # UI declaration/Actions
    module_ui_base                 # Start the module, load file location or window module_ui_base
    module_ui_qt                   # Start the module, load module_ui_qt implementation of module_ui_base
    module_viz_scene3d                # Start the module, allow to use viz_scene3d
    module_viz_scene3dQt              # Enable Ogre to render things in Qt window

    # Objects declaration
    module_service

    # Reader
    module_io_matrix
    module_io_vtk
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
        Tuto04MeshGeneratorCpp_AppCfg
) # Main application's configuration to launch
