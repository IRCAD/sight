set( NAME imageFilterOp )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwData
    fwDataTools
    fwItkIO
    fwTools
)
set( REQUIREMENTS)

set( CONAN_DEPS
    ${CONAN_ITK}
    ${CONAN_GLM}
)
