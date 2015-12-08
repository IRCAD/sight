
set( NAME Tuto11LaunchBasicConfig )
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
    vtkSimpleNegato
    uiIO
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES TutoLaunchBasicConfig)
