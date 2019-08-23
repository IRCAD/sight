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
    eigenTools
    calibration3d
)

set( REQUIREMENTS )
set( CONAN_DEPS
    ${CONAN_EIGEN}
    ${CONAN_OPENCV}
)
