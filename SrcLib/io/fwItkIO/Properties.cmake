
set( NAME fwItkIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwCore fwData fwDataIO fwTools )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    ${CONAN_ITK}
)

if(WIN32)
    list(APPEND CONAN_DEPS 
        ${CONAN_ZLIB}
        ${CONAN_LIBJPEG}
    )
endif()

if(APPLE)
    list(APPEND CONAN_DEPS 
        ${CONAN_LIBJPEG}
    )
endif()