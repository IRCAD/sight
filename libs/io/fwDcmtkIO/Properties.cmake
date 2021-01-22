set( NAME fwDcmtkIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    core
    fwData
    fwDataIO
    fwDicomIOFilter
    fwDicomTools
    fwLog
    fwMedData
    fwMedDataTools
    fwServices
    fwZip
    )
set( REQUIREMENTS )
set( USE_PCH_FROM_TARGET pchServices )
