
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
    ${CONAN_BOOST}
    ${CONAN_OPENCV}
    ${CONAN_FFMPEG}
)
