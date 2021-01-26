set( NAME opUltrasound )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
        arServices
        data
        fwDataTools
        core
        services
        fwMath
        )
set( REQUIREMENTS data module_services)
set( CONAN_DEPS
        ${CONAN_VTK}
        )