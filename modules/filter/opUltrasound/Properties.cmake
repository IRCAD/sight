set( NAME opUltrasound )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
        arServices
        data
        fwDataTools
        core
        fwServices
        fwMath
        )
set( REQUIREMENTS data servicesReg)
set( CONAN_DEPS
        ${CONAN_VTK}
        )