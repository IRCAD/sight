
set( NAME fwZip )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwCore )
set( REQUIREMENTS  )

if(WIN32)
    set( CONAN_DEPS
        zlib/1.2.11@sight/stable
    )
endif()