
set( NAME ARCalibration )
set( VERSION 0.6 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    arDataReg
    servicesReg

    patchMedicalData
    arPatchMedicalData

    gui
    guiQt

    ioAtoms
    ioCalibration
    ioActivity

    uiIO
    uiGenericQt
    uiMedDataQt

    media
    arMedia

    activities
    calibrationActivity
    trackedCameraRegistrationActivity
    toolCalibrationActivity

    appXml
    fwlauncher
    memory

    preferences
    uiPreferences
    )

bundleParam(appXml PARAM_LIST config parameters PARAM_VALUES ARCalibrationBase ARCalibrationAppBase)
