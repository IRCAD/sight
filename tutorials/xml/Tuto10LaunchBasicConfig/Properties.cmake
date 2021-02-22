set( NAME Tuto10LaunchBasicConfig )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_ui_base             # Start the module, load file location or window module_ui_base
    module_ui_qt                   # Start the module, load module_ui_qt implementation of module_ui_base

    # Objects declaration
    data
    module_service             # fwService

    # UI declaration/Actions
    module_ui_base

    # Reader
    module_io_matrix
    module_io_vtk

    # Services
    module_viz_sample
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
    Tuto10LaunchBasicConfig_AppCfg
) # Main application's configuration to launch
