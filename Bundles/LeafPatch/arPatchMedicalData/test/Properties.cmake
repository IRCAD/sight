
set( NAME arPatchMedicalDataTest )
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
    fwGui
)
set( REQUIREMENTS dataReg servicesReg ioAtoms gui patchMedicalData arPatchMedicalData)

set(CPPUNITTEST_OPTIONS BUNDLE arPatchMedicalData WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
