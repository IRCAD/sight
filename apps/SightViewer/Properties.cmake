set( NAME SightViewer )
set( VERSION 0.5 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    module_appXml                  # XML configurations

    module_ui_base             # Start the module, load file location or window module_ui_base
    module_viz_scene3d                # Start the module, allow to use viz_scene3d
    module_ui_qt                   # Start the module, allow dark theme
        # Patch old atoms data

    module_viz_scene3dQt              # Enable Ogre to render things in Qt window.

    # Objects declaration
    module_service
    data
    viz_scene2d
    io_dimse

    # UI declaration/Actions
    module_ui_base
    module_ui_viz
    module_ui_flaticons

    # Reader
    module_io_vtk
    module_io_atoms
    module_io_dicom

    # Services
    module_viz_scene2d
    module_data
    module_filter_image
    module_io_dimse

    # Configuration launchers
    config_viz_scene3d
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
        SightViewer_AppCfg
) # Main application's configuration to launch
