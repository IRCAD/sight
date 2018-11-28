
set( NAME videoPCL )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
        fwCore
        fwData
        fwRuntime
        fwServices
        fwTools
        arData
        fwCom
        fwGui
        arServices
        arPreferences
)
set( REQUIREMENTS  )
set( CONAN_DEPS
    boost/1.67.0@sight/stable
    pcl/1.8.1-r1@sight/stable
)