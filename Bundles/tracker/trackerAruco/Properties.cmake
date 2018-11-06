set( NAME trackerAruco )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwRuntime
    fwCore
    fwData
    fwDataTools
    fwServices
    fwTools
    fwMath
    fwCom
    arData
    fwMedData
    arServices
    cvIO

)
set( REQUIREMENTS dataReg
                  servicesReg
                  arDataReg
)

set( CONAN_DEPS
    opencv/3.4.3-r1@sight/stable
)