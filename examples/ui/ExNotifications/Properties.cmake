
set( NAME ExNotifications )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES 
    ui_base )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    module_ui_base             # Start the module, load file location or window module_ui_base
    module_ui_qt                   # Start the module, allow dark theme

    # Objects declaration
    module_service             # fwService

    # UI declaration/Actions
    module_ui_base

    # Services
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
        ExNotifications_AppCfg
) # Main application's configuration to launch
