set( NAME runtimeDetailTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES core ) # Don't add as dependency, in CMakeLists.txt we link to_obj to access the private api in unit-test.
set( REQUIREMENTS fwData servicesReg )
set( WARNINGS_AS_ERRORS ON )
