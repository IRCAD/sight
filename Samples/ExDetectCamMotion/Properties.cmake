
set( NAME ExDetectCamMotion )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    arDataReg
    arMedia
    ctrlCamp
    dataReg
    fwlauncher
    gui
    guiQt
    uiPreferences
    uiTools
    uiMeasurement
    maths
    preferences
    servicesReg
    videoQt
    videoTools
    visuVTK
    visuVTKAdaptor
    visuVTKARAdaptor
    visuVTKQt
    )

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExDetectCamMotionConfig)
