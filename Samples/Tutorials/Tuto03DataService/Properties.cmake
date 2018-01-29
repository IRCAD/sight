
set( NAME Tuto03DataService )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    ioVTK
    uiIO # contains services to show dialogs for reader/writer selection
    visuVTKQt
    vtkSimpleNegato
    fwlauncher
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoDataServiceConfig)
