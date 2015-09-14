
set( NAME TutoOgreGenericScene )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES )
set( REQUIREMENTS
    io
    ioVTK
    appXml
    gui
    guiQt
    ctrlSelection
    ioData
    dataReg
    servicesReg
    uiVisuQt
    uiImageQt
    uiIO
    launcher
    visuOgre
    visuOgreQt
    visu
    visuOgreAdaptor
    visuVTKAdaptor
    visuVTK
    visuVTKQt
    material
)

bundle_param(appXml PARAM_LIST config PARAM_VALUES TutoOgreGenericScene)
