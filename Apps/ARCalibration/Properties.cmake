
set( NAME ARCalibration )
set( VERSION 0.3 )
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

    io
    ioAtoms
    ioCalibration
    ioARActivity

    uiIO
    uiGenericQt
    uiMedDataQt

    media
    arMedia

    activities
    calibrationActivity
    trackedCameraRegistrationActivity

    appXml
    fwlauncher
    memory

    preferences
    uiPreferences
    )

bundleParam(appXml PARAM_LIST config parameters PARAM_VALUES ARCalibrationBase ARCalibrationAppBase)
