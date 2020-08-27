
set( NAME ExSimpleARCVOgre )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS

    appXml
    arData
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
    media
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

moduleParam(appXml PARAM_LIST config PARAM_VALUES ExSimpleARCVOgreConfig)
