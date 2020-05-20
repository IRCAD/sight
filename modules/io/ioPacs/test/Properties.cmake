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
    dataReg
    servicesReg
    gui
    ioPacs
    )

set( CONAN_DEPS
    ${CONAN_CPPUNIT}
)
