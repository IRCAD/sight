
set( NAME fwMedDataTools )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwCom
    fwData
    fwMedData
)
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    gdcm/2.8.6@sight/stable
)