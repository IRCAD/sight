
set( NAME Tuto02DataServiceBasic )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    io
    appXml
    gui
    guiQt
    dataReg
    servicesReg
    launcher
    visuVTK
    visuVTKQt
    ioData
    ioVTK
    vtkSimpleNegato
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoDataServiceBasicConfig)
