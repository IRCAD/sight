
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
    visu
    launcher
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
    )

bundle_param(appXml PARAM_LIST config PARAM_VALUES VideoTracking)

set(START_BUNDLES
    extDataReg
    arDataReg
    uiTracking
    visuVTK
    visuVTKQt
    guiQt
    arMedia
    appXml

)