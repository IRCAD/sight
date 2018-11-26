
set( NAME videoCalibration )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    arData
    arServices
    calibration3d
    fwCom
    fwCore
    fwData
    fwDataTools
    fwGui
    fwRuntime
    fwServices
    fwTools
    fwPreferences
    cvIO
)
set( REQUIREMENTS dataReg servicesReg)
set( CONAN_DEPS
    opencv/3.4.3-r1@sight/stable
)