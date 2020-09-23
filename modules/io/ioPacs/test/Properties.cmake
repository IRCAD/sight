set( NAME ioPacsTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES
    fwCore
    fwRuntime
    fwServices
    fwPacsIO
    )
set( REQUIREMENTS
    fwData
    servicesReg
    gui
    ioPacs
    )

set( CONAN_DEPS
    ${CONAN_CPPUNIT}
)
