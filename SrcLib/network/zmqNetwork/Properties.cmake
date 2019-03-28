
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
    ${CONAN_OPENIGTLINK}
    ${CONAN_ZEROMQ}
)