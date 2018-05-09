
set( NAME ARCalibration )
set( VERSION 0.4 )
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
    ioARActivity

    uiIO
    uiGenericQt
    uiMedDataQt

    media
    arMedia

    activities
    calibrationActivity
    trackedCameraRegistrationActivity
    handEyeActivity
    toolCalibrationActivity

    appXml
    fwlauncher
    memory

    preferences
    uiPreferences
    )

bundleParam(appXml PARAM_LIST config parameters PARAM_VALUES ARCalibrationBase ARCalibrationAppBase)
