
set( NAME fwQt )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwCore fwData fwGui fwQml fwGuiQml fwRuntime fwServices fwThread fwTools )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET pchServices)
set( CONAN_DEPS
    ${CONAN_QT}
)
