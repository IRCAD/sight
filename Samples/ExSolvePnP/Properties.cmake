
set( NAME ExSolvePnP )
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
    ioCalibration
    ioVTK
    uiPreferences
    uiTools
    uiMeasurement
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

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExSolvePnPConfig)
