
set( NAME Tuto15Multithread )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwData
    servicesReg
    gui
    guiQt
    ioData
    ioVTK
    uiIO
    visuVTKQt
    vtkSimpleMesh
    fwlauncher
    appXml
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES tuto15MultithreadConfig)
