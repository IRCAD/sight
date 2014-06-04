
set( NAME guiQtTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwTest fwCore fwTools fwData fwGuiQt )
set( REQUIREMENTS dataReg servicesReg gui guiQt io ioVTK visu visuVTKQt vtkSimpleNegato Tuto01Basic Tuto02DataServiceBasic )

set(CPPUNITTEST_OPTIONS BUNDLE guiQt WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
