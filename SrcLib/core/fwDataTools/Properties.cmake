
set( NAME fwDataTools )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwData fwMath fwRuntime fwServices)
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    ${CONAN_GLM}
)