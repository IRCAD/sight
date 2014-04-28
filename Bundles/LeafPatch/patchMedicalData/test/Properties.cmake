
set( NAME patchMedicalDataTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES fwTest fwCore fwTools fwData fwDataTools fwStructuralPatch fwMDSemanticPatch )
set( REQUIREMENTS dataReg servicesReg ioAtoms gui patchMedicalData )

set(CPPUNITTEST_OPTIONS BUNDLE patchMedicalData)
