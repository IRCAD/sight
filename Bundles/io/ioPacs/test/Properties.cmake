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

set(CPPUNITTEST_OPTIONS BUNDLE ioPacs WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set( CONAN_DEPS
    cppunit/1.14.0@sight/stable
)