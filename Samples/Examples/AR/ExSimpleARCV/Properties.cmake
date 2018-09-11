
set( NAME ExSimpleARCV )
set( VERSION 0.3 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    arDataReg
    media
    arMedia
    ctrlCamp
    ctrlCom
    dataReg
    fwlauncher
    gui
    guiQt
    ioCalibration
    ioData
    ioVTK
    uiPreferences
    uiTools
    maths
    preferences
    servicesReg
    trackerAruco
    registrationCV
    videoQt
    videoTools
    videoCalibration
    visuVTK
    visuVTKAdaptor
    visuVTKARAdaptor
    visuVTKQt
    )

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExSimpleARCVConfig)
