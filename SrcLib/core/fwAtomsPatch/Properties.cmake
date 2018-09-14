
set( NAME fwAtomsPatch )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES fwAtomConversion fwAtoms fwCore fwTools fwRuntime )
set( REQUIREMENTS  )
set( USE_PCH_FROM_TARGET fwAtomConversion )
set( CONAN_DEPS
    boost/1.67.0@fw4spl/stable
    camp/0.8.2@fw4spl/stable
)