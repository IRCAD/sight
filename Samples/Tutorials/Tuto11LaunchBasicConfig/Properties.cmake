
set( NAME Tuto11LaunchBasicConfig )
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
    visuVTK
    visuVTKQt
    vtkSimpleNegato
    fwlauncher
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES TutoLaunchBasicConfig)
