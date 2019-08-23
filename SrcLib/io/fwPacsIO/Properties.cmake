
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
    fwRuntime
)
set( REQUIREMENTS )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    ${CONAN_DCMTK}
)
