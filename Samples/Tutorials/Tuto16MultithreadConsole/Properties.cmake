
set( NAME Tuto16MultithreadConsole )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES
    fwDataTools
    fwServices
)
set( REQUIREMENTS
    dataReg
    servicesReg
    fwlauncher
    appXml
    # The bundle console is used to run a console application with a main loop
    # Typically it can be used for server command line applications
    console
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Tuto16MultithreadConsoleConfig)
set( CONAN_DEPS
    boost/1.67.0@sight/stable
)