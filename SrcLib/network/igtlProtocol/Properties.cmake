
set( NAME igtlProtocol )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES   
    fwCore
    fwRuntime
    fwData
    fwServices
    fwTools
    fwZip
    fwAtoms
    fwAtomConversion
    fwAtomsBoostIO
    fwDataTools
)
set( REQUIREMENTS  )

set( CONAN_DEPS
    ${CONAN_LIBARCHIVE}
    ${CONAN_OPENIGTLINK}
)