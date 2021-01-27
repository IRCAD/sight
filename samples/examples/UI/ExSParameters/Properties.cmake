set( NAME ExSParameters )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    module_guiQt                   # Start the module, allow dark theme

    # Objects declaration
    module_services             # fwService

    # UI declaration/Actions
    module_gui
    style
)

moduleParam(module_guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_guiQt

moduleParam(
        appXml
    PARAM_LIST
        config
    PARAM_VALUES
        ExSParameters_AppCfg
) # Main application's configuration to launch
