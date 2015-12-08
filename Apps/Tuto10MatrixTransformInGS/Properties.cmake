
set( NAME Tuto10MatrixTransformInGS )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    io
    gui
    guiQt
    dataReg
    servicesReg
    launcher
    ioVTK
    ioData
    uiIO
    uiVisuQt
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tuto)
