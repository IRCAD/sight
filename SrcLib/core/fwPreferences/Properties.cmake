set( NAME fwPreferences )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwTools
    fwRuntime
    fwData
    fwServices
)
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchServices)

if(WIN32)
set( CONAN_DEPS
    ${CONAN_OPENSSL}
)
endif()
