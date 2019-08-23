
set( NAME monitorQt )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES fwCom fwCore fwData fwGui fwGuiQt fwMemory fwRuntime fwServices fwTools gui )
set( REQUIREMENTS  )
set( CONAN_DEPS
    ${CONAN_QT}
)

if(WIN32)
    list(APPEND CONAN_DEPS ${CONAN_LIBXML2})
endif()