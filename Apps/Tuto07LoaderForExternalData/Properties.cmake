
set( NAME Tuto07LoaderForExternalData )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS 
    io
    gui
    guiQt
    dataReg
    servicesReg
    visu
    visuVTK
    visuVTKQt
    launcher
    ioVTK
    ioAtoms
    ioTuto
    ioData
    vtkSimpleNegato
    uiIO
    ctrlSelection
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES tutoLoaderExternalDataConfig)
