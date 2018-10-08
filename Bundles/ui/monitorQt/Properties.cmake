
set( NAME monitorQt )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES fwCom fwCore fwData fwGui fwGuiQt fwMemory fwRuntime fwServices fwTools gui )
set( REQUIREMENTS  )

set( CONAN_DEPS
    qt/5.11.1@sight/stable
    libxml2/2.9.8@sight/stable
)