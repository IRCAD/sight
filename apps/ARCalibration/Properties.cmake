set( NAME ARCalibration )
set( VERSION 0.8 )
set( TYPE APP )
set( DEPENDENCIES  )
set( REQUIREMENTS
    fwlauncher              # Just to build the launcher
    module_appXml                  # XML configurations

    module_ui_base             # Start the module, load file location or window module_ui_base
    module_ui_qt                   # Start the module, allow dark theme
        # Patch old atoms data
    arPatchMedicalData      # Patch old atoms data

    # Objects declaration
    module_services             # fwService
    data

    # UI declaration/Actions
    module_ui_base
    module_ui_flaticon
    media

    # Reader
    module_io_atoms
    module_io_vision
    ioActivity
    module_io_realsense

    # Services
    module_ui_base
    module_activity
    calibrationActivity
    trackedCameraRegistrationActivity
    toolCalibrationActivity
    module_data
    module_memory
    )

moduleParam(module_ui_qt
    PARAM_LIST
        resource
        stylesheet
    PARAM_VALUES
        module_ui_qt-0.1/flatdark.rcc
        module_ui_qt-0.1/flatdark.qss
) # Allow dark theme via module_ui_qt

moduleParam(
        module_appXml
    PARAM_LIST
        config parameters
    PARAM_VALUES
        ARCalibration_AppCfg ARCalibration_AppCfgParam
) # Main application's configuration to launch
