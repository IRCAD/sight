
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
    fwCom
    fwGui
    fwIO
)
set( REQUIREMENTS dataReg servicesReg)
set( CONAN_DEPS
    opencv/3.4.3-r3@sight/testing
)