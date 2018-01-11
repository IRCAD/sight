
set( NAME filterVRRenderTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES 
    fwAtoms
    fwAtomsFilter
    fwAtomsPatch
    fwTools
    fwCore
    fwRuntime
    fwTest
    )
set( REQUIREMENTS filterVRRender dataReg servicesReg)

set(CPPUNITTEST_OPTIONS BUNDLE filterVRRender WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
