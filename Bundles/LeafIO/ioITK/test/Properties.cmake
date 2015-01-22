
set( NAME ioITKTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwTest fwTools fwRuntime fwData fwMedData fwServices)
set( REQUIREMENTS ioITK )

set(CPPUNITTEST_OPTIONS BUNDLE ioITK WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
