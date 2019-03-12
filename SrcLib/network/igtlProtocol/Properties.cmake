
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
    libarchive/3.1.2@sight/stable
    openigtlink/2.1-r1@sight/stable
)