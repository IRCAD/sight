
set( NAME fwZip )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwCore )
set( REQUIREMENTS  )

if(WIN32)
    set( CONAN_DEPS
        ${CONAN_ZLIB}
    )
endif()