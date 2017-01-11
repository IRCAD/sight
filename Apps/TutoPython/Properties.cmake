
set( NAME TutoPython )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES )
set( REQUIREMENTS 
    fwPython
    io
    ioVTK
    uiIO
    gui
    guiQt
    binding
    uiPython
    dataReg
    servicesReg
    appXml
    fwlauncher
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoPythonConfig)
