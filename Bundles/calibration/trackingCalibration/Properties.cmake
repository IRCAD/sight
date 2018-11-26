set( NAME trackingCalibration )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwRuntime
    fwCore
    fwData
    fwDataTools
    fwGuiQt
    fwServices
    fwTools
    fwPreferences
    fwMath
    fwCom
    arData
    cvIO
    eigenTools
    calibration3d
)

set( REQUIREMENTS )
set( CONAN_DEPS
    eigen/3.3.4@sight/stable
    opencv/3.4.3-r1@sight/stable
)
