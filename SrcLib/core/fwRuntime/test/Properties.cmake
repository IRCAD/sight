
set( NAME fwRuntimeTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwRuntime )
set( REQUIREMENTS dataReg servicesReg )
set(CPPUNITTEST_OPTIONS WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
set( CONAN_DEPS
    cppunit/1.14.0@fw4spl/stable
)