
set( NAME videoOpenCV )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
        fwCore
        fwData
        fwRuntime
        fwServices
        fwTools
        fwIO
        arData
        fwCom
        fwGui
        arServices
        arPreferences 
)
set( REQUIREMENTS  )
set( CONAN_DEPS
    boost/1.67.0@sight/stable
    opencv/3.4.3@sight/stable
)