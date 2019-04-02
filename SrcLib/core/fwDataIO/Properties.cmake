
set( NAME fwDataIO )
set( VERSION 0.2 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwData
    fwDataTools
    fwTools
    fwJobs
)
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )

if(WIN32)
    set( CONAN_DEPS
        ${CONAN_ZLIB}
    )
endif()