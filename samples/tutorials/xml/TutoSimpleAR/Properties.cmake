
set( NAME TutoSimpleAR )
set( VERSION 0.4 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    appXml
    arData
    media
    ctrlCamp
    ctrlCom
    fwData
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

moduleParam(appXml PARAM_LIST config PARAM_VALUES TutoSimpleARConfig)
