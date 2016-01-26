
set( NAME ARCalibration )
set( VERSION 0.2 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    extDataReg
    arDataReg
    servicesReg
    gui
    guiQt
    io
    ioAtoms
    patchMedicalData
    arPatchMedicalData
    uiIO
    uiGenericQt
    uiMedDataQt
    uiImageQt
    ctrlSelection
    memory
    media
    mediaExt
    arMedia
    visuVTK
    visuVTKQt
    videoQt
    visuVTKAdaptor
    visuVTKARAdaptor
    activities
    calibrationActivity
    uiCalibration
    appXml
    preferences
    videoTools
    ioCalibration
    videoCalibration
    launcher
    uiTools
    trackerAruco
    )

bundleParam(appXml PARAM_LIST config parameters PARAM_VALUES ARCalibrationBase ARCalibrationAppBase)
