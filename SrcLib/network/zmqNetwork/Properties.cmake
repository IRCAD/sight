
set( NAME zmqNetwork )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES   fwCore
                    fwData
                    fwTools
                    igtlProtocol
)
set( REQUIREMENTS  )

set( CONAN_DEPS
    openigtlink/2.1@fw4spl/stable
    zeromq/4.0.5@fw4spl/stable
)