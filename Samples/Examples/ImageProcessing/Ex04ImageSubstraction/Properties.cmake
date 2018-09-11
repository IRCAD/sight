
set( NAME Ex04ImageSubstraction)
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    servicesReg
    dataReg
    gui
    guiQt
    fwlauncher
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    ioVTK
    ioITK
    ioData
    uiImageQt
    uiVisu
    uiVisuQt
    ctrlSelection
    appXml
    basicRegistration
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES Ex04ImageSubstractionConfig)
