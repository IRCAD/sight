
set( NAME videoRealSense )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
        fwCore
        arData
        fwData
        fwDataTools
        fwCom
        fwGui
        fwRuntime
        arServices
        fwServices
        fwTools

)
set( REQUIREMENTS )
set( CONAN_DEPS
     librealsense/2.18.0@sight/stable
)

set( USE_PCH_FROM_TARGET pchServicesOmp )
