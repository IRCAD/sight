
set( NAME Tuto15Multithread )
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
    visuVTKQt
    vtkSimpleMesh
    fwlauncher
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tuto15MultithreadConfig)
