
set( NAME Tuto06Filter )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    ioVTK
    uiIO
    visuVTKQt
    vtkSimpleNegato
    opImageFilter # module containing the action to performs a threshold
    fwlauncher
    appXml
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES FilterConfig)
