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
    ioPacs
    )

set( CONAN_DEPS
    cppunit/1.14.0@sight/stable
)
