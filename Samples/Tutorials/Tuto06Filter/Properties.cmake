
set( NAME Tuto06Filter )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    io
    ioVTK
    uiIO
    visuVTKQt
    vtkSimpleNegato
    opImageFilter # bundle containing the action to performs a threshold
    fwlauncher
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES FilterConfig)
