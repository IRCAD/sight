set( NAME fwRuntimeDetailTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES core ) # Don't add fwRuntime as dependency, in CMakeLists.txt we link to fwRuntime_obj to access the private api in unit-test.
set( REQUIREMENTS fwData servicesReg )
set( WARNINGS_AS_ERRORS ON )
