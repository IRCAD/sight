set( NAME Tuto05MultithreadConsoleCpp )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES   
    data
    services
)
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    module_appXml                  # XML configurations

    # Objects declaration
    data
    module_services             # fwService

    # The module console is used to run a console application with a main loop
    # Typically it can be used for server command line applications
    console
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
        config
    PARAM_VALUES
        Tuto05MultithreadConsoleCpp_AppCfg
) # Main application's configuration to launch
