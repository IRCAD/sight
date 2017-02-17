
set( NAME Ex04SimpleAR )
set( VERSION 0.1 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS

    activities
    appXml2
    arDataReg
    arMedia
    ctrlCamp
    dataReg
    fwlauncher
    gui
    guiQt
    ioCalibration
    maths
    preferences
    servicesReg
    tracker
    trackerAruco
    uiTracking
    videoQt
    videoTools
    visuVTK
    visuVTKAdaptor
    visuVTKARAdaptor
    visuVTKQt

    )

bundleParam(appXml2 PARAM_LIST config PARAM_VALUES Ex04SimpleARConfig)
