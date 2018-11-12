
set( NAME fwGuiQt )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwCore fwData fwGui fwRuntime fwServices fwThread fwTools )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchServices)
set( CONAN_DEPS
    qt/5.11.2@sight/stable
)