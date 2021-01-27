set( NAME opUltrasound )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
        data
        core
        services
        fwMath
        )
set( REQUIREMENTS data module_services)
set( CONAN_DEPS
        ${CONAN_VTK}
        )
