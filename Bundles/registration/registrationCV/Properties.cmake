
set( NAME registrationCV )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    arData
    calibration3d
    fwCom
    fwCore
    fwData
    fwMath
    fwRuntime
    fwServices
    cvIO
)

set( REQUIREMENTS dataReg
                  servicesReg
)

set( CONAN_DEPS
    ${CONAN_OPENCV}
)