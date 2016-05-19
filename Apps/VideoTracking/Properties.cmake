
set( NAME VideoTracking )
set( VERSION 0.1 )
set( TYPE APP )
set( UNIQUE TRUE)
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    arDataReg
    extDataReg
    gui
    guiQt
    servicesReg
    fwlauncher
    appXml
    visuVTK
    visuVTKQt
    visuVTKAdaptor
    visuVTKARAdaptor
    videoQt
    videoTools
    tracker
    trackerAruco
    arMedia
    uiTracking
    preferences
    )

bundleParam(appXml PARAM_LIST config PARAM_VALUES VideoTracking)
