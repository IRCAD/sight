
set( NAME Tuto03DataServiceBis )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    io
    gui
    guiQt
    dataReg
    servicesReg
    visuVTK
    visuVTKQt
    launcher
    ioVTK
    ioData
    vtkSimpleMesh
    uiIO
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoDataServiceConfig)
