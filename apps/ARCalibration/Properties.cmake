set( NAME ARCalibration )
set( VERSION 0.8 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    appXml                  # XML configurations

    preferences             # Start the module, load file location or window preferences
    guiQt                   # Start the module, allow dark theme
    patchMedicalData        # Patch old atoms data
    arPatchMedicalData      # Patch old atoms data

    # Objects declaration
    dataReg                 # fwData
    servicesReg             # fwService
    arDataReg               # arData

    # UI declaration/Actions
    gui
    style
    flatIcon
    uiIO
    uiMedDataQt
    uiActivitiesQt
    media

    # Reader
    ioAtoms
    ioCalibration
    ioActivity
    videoRealSense

    # Services
    preferences
    uiPreferences
    filterUnknownSeries
    activities
    calibrationActivity
    trackedCameraRegistrationActivity
    toolCalibrationActivity
    ctrlSelection
    memory
    )

moduleParam(guiQt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via guiQt

moduleParam(
        appXml
    PARAM_LIST
        config parameters
    PARAM_VALUES
        ARCalibration_AppCfg ARCalibration_AppCfgParam
) # Main application's configuration to launch
