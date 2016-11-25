
set( NAME fwServicesTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwTest fwTools fwRuntime fwData fwServices fwCom fwThread )
set( REQUIREMENTS dataReg servicesReg )
set( CPPUNITTEST_OPTIONS BUNDLE fwServices WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
