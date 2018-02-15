
set( NAME patchMedicalDataTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES
    fwTest
    fwCore
    fwData
    fwServices
    fwRuntime
    fwMedData
    fwThread
    fwMDSemanticPatch
)
set( REQUIREMENTS dataReg servicesReg ioAtoms gui guiQt patchMedicalData )

set(CPPUNITTEST_OPTIONS BUNDLE patchMedicalData WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
