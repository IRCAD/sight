
set( NAME ioAtomsTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES
    fwCore 
    fwTest
    fwTools
    fwData
    fwDataCamp
    fwServices
    fwRuntime
    fwMedData
    fwGui
    fwThread
)
set( REQUIREMENTS ioAtoms)

set(CPPUNITTEST_OPTIONS BUNDLE ioAtoms WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
