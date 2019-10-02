
set( NAME ioCalibration )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwCore
    fwCom
    fwData
    fwRuntime
    fwServices
    fwTools
    arData
    fwGui
    fwPreferences
    fwIO
    cvIO
    calibration3d
)
set( REQUIREMENTS dataReg servicesReg)
set( CONAN_DEPS
    ${CONAN_OPENCV}
)