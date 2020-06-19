
set( NAME fwToolsTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwTools)
set( REQUIREMENTS  )
set( WARNINGS_AS_ERRORS ON)
set( CONAN_DEPS
    ${CONAN_BOOST}
    ${CONAN_CAMP} # Needed, loaded dynamically in the unit-test
    ${CONAN_CPPUNIT}
)
