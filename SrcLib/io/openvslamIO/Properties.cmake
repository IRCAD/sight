set( NAME openvslamIO)
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    arData
    fwCore
    fwCom
    fwData
    fwDataTools
)
set( REQUIREMENTS )

set( CONAN_DEPS
    ${CONAN_OPENCV}
    ${CONAN_OPENVSLAM}
)
