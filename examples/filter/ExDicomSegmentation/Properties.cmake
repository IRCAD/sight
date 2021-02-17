set( NAME ExDicomSegmentation )
set( VERSION 0.2 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_ui_base             # Start the module, load file location or window module_ui_base
    module_viz_scene3d                # Start the module, allow to use viz_scene3d
    module_ui_qt                   # Start the module, allow dark theme
    module_viz_scene3dQt              # Enable Ogre to render things in Qt window

    # Extensions
    module_activity

    # Objects declaration
    data
    module_service             # fwService

    # UI declaration/Actions
    module_ui_base
    module_ui_flaticon

    # Reader
    module_io_activity
    module_io_matrix
    module_io_dicom
    module_io_vtk
    module_io_atoms

    # Config
    3DVisualizationActivity
    2DVisualizationActivity

    # Services
    module_filter_image
    module_data
    module_filter_mesh

    # Generic Scene
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
        ExDicomSegmentation_AppCfg
) # Main application's configuration to launch

