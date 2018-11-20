
set( NAME fwItkIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwCore fwData fwDataIO fwTools )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchData )
set( CONAN_DEPS
    itk/4.13.0-r1@sight/stable
)
if(WIN32 OR APPLE)
    list(APPEND CONAN_DEPS 
        zlib/1.2.11@sight/stable
        libjpeg/9c@sight/stable
    )
endif()