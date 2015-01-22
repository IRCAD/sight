
set( NAME ioVTKTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwTest fwTools fwData fwMedData fwDataCamp fwComEd fwServices fwRuntime )
set( REQUIREMENTS ioVTK )

set(CPPUNITTEST_OPTIONS BUNDLE ioVTK WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
