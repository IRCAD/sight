
set( NAME TutoMatrix )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS 
    appXml
    gui
    guiQt
    dataReg
    servicesReg
    fwlauncher
    maths
    fwRenderVTK
    uiIO
    visuVTKQt
    visuVTKAdaptor
    uiVisuQt
    ioVTK
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES TutoMatrixConfig)
