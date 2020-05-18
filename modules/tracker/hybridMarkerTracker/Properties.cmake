set(NAME hybridMarkerTracker)
set(VERSION 0.1)
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
        ${CONAN_OPENCV}
        ${CONAN_IPPE}
        ${CONAN_HYBRIDMARKERTRACKER}
        )