
set( NAME fwDcmtkIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCom
    fwCore
    fwDcmtkTools
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
    boost/1.67.0@sight/stable
    dcmtk/3.6.3@sight/stable
)