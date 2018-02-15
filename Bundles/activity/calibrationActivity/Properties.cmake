
set( NAME calibrationActivity )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCore
    fwData
    fwActivities
)
set( REQUIREMENTS
    ctrlCamp

    servicesReg
    dataReg
    arDataReg

    gui

    ioData
    ioCalibration

    visuVTKQt
    visuVTKAdaptor
    visuVTKARAdaptor

    arMedia
    media

    uiTools
    uiIO
    uiCalibration
    uiPreferences

    videoCalibration
    videoOpenCV
    videoOpenni
    videoQt
    videoTools

    trackerAruco

    validators

    preferences
)
