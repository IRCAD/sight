set( NAME fwRuntimeDetailTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore ) # Don't add fwRuntime as dependency, in CMakeLists.txt we link to fwRuntime_obj to access the private api in unit-test.
set( REQUIREMENTS dataReg servicesReg )
set( WARNINGS_AS_ERRORS ON )

set( CONAN_DEPS
    ${CONAN_CPPUNIT}
)
