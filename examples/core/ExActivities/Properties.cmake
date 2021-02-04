set( NAME ExActivities )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_ui_base             # Start the module, load file location or window module_ui_base
    module_ui_qt                   # Start the module, allow dark theme

    # Extensions
    activities

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_ui_base
    module_ui_flaticon

    # Reader
    module_io_atoms
    module_io_vtk
    ioActivity

    # Config
    2DVisualizationActivity
    volumeRenderingActivity
    activitiesConfig

    # Services

    # Generic Scene
    module_viz_sample
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
        config parameters
    PARAM_VALUES
        ExActivities_Base_AppCfg ExActivities_AppCfgParam
) # Main application's configuration to launch
