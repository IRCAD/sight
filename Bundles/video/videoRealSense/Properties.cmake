
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
        fwThread
)
set( REQUIREMENTS )
set( CONAN_DEPS
     pcl/1.8.1-r2@sight/stable
     librealsense/2.17.0@sight/stable
)
