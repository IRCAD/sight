set( NAME trackerAruco )
set( VERSION 0.1 )
set( TYPE MODULE )
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
    ${CONAN_OPENCV}
)
