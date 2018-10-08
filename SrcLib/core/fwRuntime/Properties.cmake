
set( NAME fwRuntime )
set( VERSION 0.3 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwCore )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET fwCore )
set( CONAN_DEPS
    boost/1.67.0@sight/stable
)
if(WIN32)
    list(APPEND CONAN_DEPS winiconv/0.0.8@sight/stable)
endif()
if(WIN32 OR APPLE)
    list(APPEND CONAN_DEPS libxml2/2.9.8@sight/stable)
endif()