
set( NAME Tuto10MatrixTransformInGS )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    io
    ioData
    ioVTK
    uiIO
    uiVisuQt
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    maths
    fwlauncher
    appXml2
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Tuto10MatrixTransformInGS)
