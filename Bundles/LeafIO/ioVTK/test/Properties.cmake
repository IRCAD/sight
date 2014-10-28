
set( NAME ioVTKTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwTest fwTools fwData fwDataTools fwMedData fwDataCamp fwVtkIO )
set( REQUIREMENTS ioVTK )

set(CPPUNITTEST_OPTIONS BUNDLE ioVTK WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
