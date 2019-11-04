
set( NAME TutoOgreGenericScene )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES )
set( REQUIREMENTS
    ioVTK
    appXml
    gui
    guiQt
    ioData
    dataReg
    servicesReg
    uiImageQt
    uiIO
    fwlauncher
    visuOgre
    visuOgreQt
    visuOgreAdaptor
    material
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES TutoOgreGenericScene)
