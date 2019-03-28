
set( NAME fwAtomsPatch )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwAtomConversion fwAtoms fwCore fwTools fwRuntime )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET fwAtomConversion )
set( CONAN_DEPS
    ${CONAN_BOOST}
    ${CONAN_CAMP}
)