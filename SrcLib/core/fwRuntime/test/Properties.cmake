set( NAME fwRuntimeTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwRuntime )
set( REQUIREMENTS dataReg servicesReg )
set( WARNINGS_AS_ERRORS ON )

set( CONAN_DEPS
    ${CONAN_CPPUNIT}
)
