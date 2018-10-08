
set( NAME fwPacsIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwTools
    fwThread
    fwData
    fwMedData
    fwCom
    fwDcmtkTools
    )
set( REQUIREMENTS )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    dcmtk/3.6.3@sight/stable
)