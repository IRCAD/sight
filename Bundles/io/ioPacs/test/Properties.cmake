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
    io
    ioPacs
    )

set(CPPUNITTEST_OPTIONS BUNDLE ioPacs WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
