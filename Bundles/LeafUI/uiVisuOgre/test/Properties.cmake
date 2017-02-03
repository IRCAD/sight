
set( NAME uiVisuOgreTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwCore fwTest fwTools )
set( REQUIREMENTS dataReg servicesReg io gui uiIO uiVisuOgre )

set(CPPUNITTEST_OPTIONS BUNDLE uiVisuOgre WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
