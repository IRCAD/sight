
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
    arMDSemanticPatch
)
set( REQUIREMENTS dataReg servicesReg ioAtoms patchMedicalData arPatchMedicalData)
