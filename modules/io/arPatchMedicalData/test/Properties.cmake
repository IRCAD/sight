
set( NAME arPatchMedicalDataTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES
    fwTest
    core
    fwData
    fwServices
    fwRuntime
    fwMedData
    fwThread
    arMDSemanticPatch
)
set( REQUIREMENTS fwData servicesReg ioAtoms patchMedicalData arPatchMedicalData)
