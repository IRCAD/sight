
set( NAME fwToolsTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwTools)
set( REQUIREMENTS  )

set( CONAN_DEPS
    boost/1.67.0@sight/stable
    camp/0.8.2@sight/stable # Needed, loaded dynamically in the unit-test
    cppunit/1.14.0@sight/stable
)
