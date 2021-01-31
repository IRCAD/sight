set( NAME ARCalibration )
set( VERSION 0.8 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    module_appXml                  # XML configurations

    module_io_base             # Start the module, load file location or window module_io_base
    module_ui_qt                   # Start the module, allow dark theme
    patchMedicalData        # Patch old atoms data
    arPatchMedicalData      # Patch old atoms data

    # Objects declaration
    module_services             # fwService
    data

    # UI declaration/Actions
    module_ui_base
    flatIcon
    uiIO
    uiMedDataQt
    media

    # Reader
    module_io_atoms
    ioCalibration
    ioActivity
    videoRealSense

    # Services
    module_io_base
    uiPreferences
    filterUnknownSeries
    activities
    calibrationActivity
    trackedCameraRegistrationActivity
    toolCalibrationActivity
    ctrlSelection
    memory
    )

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        style-0.1/flatdark.rcc
        style-0.1/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config parameters
    PARAM_VALUES
        ARCalibration_AppCfg ARCalibration_AppCfgParam
) # Main application's configuration to launch
