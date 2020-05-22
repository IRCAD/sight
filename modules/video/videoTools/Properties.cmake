
set( NAME videoTools )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
        fwCore
        fwData
        fwDataTools
        fwGui
        fwMath
        fwRuntime
        fwServices
        fwTools
        arData
        fwCom
        arServices
        cvIO
)
set( REQUIREMENTS  )

set( CONAN_DEPS
    ${CONAN_OPENCV}
)