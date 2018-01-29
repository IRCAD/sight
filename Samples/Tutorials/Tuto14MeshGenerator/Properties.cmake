
set( NAME Tuto14MeshGenerator )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES
    fwDataTools
    fwGui )
set( REQUIREMENTS
    dataReg
    gui
    guiQt
    ioData
    ioVTK
    servicesReg
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    fwlauncher
    appXml
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Tuto14MeshGenerator)
