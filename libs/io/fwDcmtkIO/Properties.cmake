set( NAME fwDcmtkIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCom
    core
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
