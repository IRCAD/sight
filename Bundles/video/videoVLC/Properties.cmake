
set( NAME videoVLC )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    arData
    arPreferences
    arServices
    fwCom
    fwCore
    fwData
    fwDataTools
    fwRuntime
    fwServices
    fwTools
)
set( REQUIREMENTS )

if(WIN32 OR APPLE)
  set( CONAN_DEPS vlc/3.0.4-r1@sight/stable )
endif()
