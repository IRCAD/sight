
set( NAME ExSimpleARCVOgre )
set( VERSION 0.2 )
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
    visuOgre
    visuOgreAdaptor
    visuOgreQt
)

bundleParam(appXml PARAM_LIST config PARAM_VALUES ExSimpleARCVOgreConfig)
