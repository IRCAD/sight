
set( NAME ExImageMix )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwData
    ctrlSelection
    gui
    guiQt
    ioData
    ioVTK
    servicesReg
    uiVisuQt
    uiImageQt
    uiIO
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    fwlauncher
    appXml
)

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExImageMix)
