
set( NAME fwDcmtkIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCom
    fwCore
    fwData
    fwDataIO
    fwDicomIOFilter
    fwDicomTools
    fwTools
    fwThread
    fwLog
    fwMedData
    fwMedDataTools
    fwMemory
    fwServices
    fwZip
    )
set( REQUIREMENTS )
set( USE_PCH_FROM_TARGET pchServices )
set( CONAN_DEPS
    ${CONAN_BOOST}
    ${CONAN_DCMTK}
)
