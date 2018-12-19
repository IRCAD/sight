
set( NAME fwToolsTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwTools fwTest)
set( REQUIREMENTS  )

set( CONAN_DEPS
     camp/0.8.2@sight/stable # Needed, loaded dynamically in the unit-test
)
