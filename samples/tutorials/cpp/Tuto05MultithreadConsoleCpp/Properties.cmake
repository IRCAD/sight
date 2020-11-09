set( NAME Tuto05MultithreadConsoleCpp )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES   
    fwCom
    fwData
    fwRuntime
    fwServices
    fwThread
)
set( REQUIREMENTS
    fwlauncher              # Needed to build the launcher
    appXml                  # XML configurations

    # Objects declaration
    fwData
    servicesReg             # fwService

    # The module console is used to run a console application with a main loop
    # Typically it can be used for server command line applications
    console
)

moduleParam(guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via guiQt

moduleParam(
        appXml
    PARAM_LIST
        config
    PARAM_VALUES
        Tuto05MultithreadConsoleCpp_AppCfg
) # Main application's configuration to launch

set( CONAN_DEPS
    ${CONAN_BOOST}
)
