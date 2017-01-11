
set( NAME Tuto03DataService )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    servicesReg
    gui
    guiQt
    io
    ioVTK
    uiIO # contains services to show dialogs for reader/writer selection
    visuVTKQt
    vtkSimpleNegato
    fwlauncher
    appXml2
)

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES tutoDataServiceConfig)
