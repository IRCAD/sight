
set( NAME fwActivities )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwCore fwData fwDataCamp fwMath fwMedData fwRuntime fwTools fwMedDataTools )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    ${CONAN_BOOST}
)
