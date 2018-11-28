set( NAME itkRegistrationOp )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwTools
    fwItkIO
    fwData
    fwCom
    fwCore
    fwDataTools
)
set( REQUIREMENTS)

set( CONAN_DEPS
    itk/4.13.0-r1@sight/stable
)