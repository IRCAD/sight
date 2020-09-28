
set( NAME ExStereoARCV )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    arData
    media
    ctrlCamp
    fwData
    fwlauncher
    gui
    guiQt
    ioCalibration
    ioTimeline
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

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExStereoARCVConfig)
