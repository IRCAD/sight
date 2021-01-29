set( NAME ExActivities )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_io_base             # Start the module, load file location or window module_io_base
    module_ui_qt                   # Start the module, allow dark theme

    # Extensions
    activities

    # Objects declaration
    data
    module_services             # fwService

    # UI declaration/Actions
    module_ui_base
    style
    flatIcon

    # Reader
    module_io_atoms
    module_io_vtk
    ioActivity

    # Config
    2DVisualizationActivity
    volumeRenderingActivity
    activitiesConfig

    # Services
    uiIO

    # Generic Scene
    visuBasic
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
        config parameters
    PARAM_VALUES
        ExActivities_Base_AppCfg ExActivities_AppCfgParam
) # Main application's configuration to launch
