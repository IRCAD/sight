
set( NAME Tuto15Multithread )
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
    ioVTK
    ioData
    vtkSimpleMesh
    uiIO
    appXml
    launcher
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tuto15MultithreadConfig)
