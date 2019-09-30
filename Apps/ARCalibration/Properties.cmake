
set( NAME ARCalibration )
set( VERSION 0.7 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    dataReg
    arDataReg
    servicesReg

    ctrlSelection

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
    uiActivitiesQt

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
