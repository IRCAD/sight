
set( NAME scene2DTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwTest fwData fwServices fwRuntime fwTools)
set( REQUIREMENTS dataReg servicesReg scene2D )

set(CPPUNITTEST_OPTIONS BUNDLE scene2D WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
