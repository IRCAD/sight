
set( NAME calibrationActivity )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCore
    fwData
    fwActivities
)
set( REQUIREMENTS
    servicesReg
    dataReg
    extDataReg
    arDataReg

    gui

    ioData

    visuVTKAdaptor
    visuVTKARAdaptor

    arMedia
    media
    mediaExt

    uiTools
    uiIO
    uiCalibration

    videoCalibration
    videoQt
    videoTools

    trackerAruco

    validators
)
