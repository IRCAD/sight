
set( NAME ExDetectCamMotion )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    arData
    fwData
    fwlauncher
    gui
    guiQt
    uiPreferences
    uiTools
    preferences
    servicesReg
    videoQt
    videoTools
    visuVTK
    visuVTKAdaptor
    visuVTKARAdaptor
    visuVTKQt
    )

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExDetectCamMotionConfig)
