
set( NAME Tuto10MatrixTransformInGS )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    ioData
    ioVTK
    uiIO
    uiVisuQt
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    fwlauncher
    appXml
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES Tuto10MatrixTransformInGS)
