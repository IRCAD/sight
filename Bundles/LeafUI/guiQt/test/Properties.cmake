
set( NAME guiQtTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwTest fwCore fwData fwRuntime fwServices)
set( REQUIREMENTS dataReg servicesReg gui guiQt io ioVTK visuVTKQt vtkSimpleNegato Tuto01Basic Tuto02DataServiceBasic appXml )

set(CPPUNITTEST_OPTIONS BUNDLE guiQt WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
