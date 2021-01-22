set( NAME opUltrasound )
set( VERSION 0.1 )
set( TYPE MODULE )
set( DEPENDENCIES
        arServices
        fwRuntime
        fwData
        fwDataTools
        core
        fwServices
        fwCom
        fwMath
        )
set( REQUIREMENTS fwData servicesReg)
set( CONAN_DEPS
        ${CONAN_VTK}
        )