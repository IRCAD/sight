set( NAME videoOrbbec )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
        arData
        arServices
        fwData
        fwServices
        fwGui
    )
set( REQUIREMENTS )
set( CONAN_DEPS
     ${CONAN_OPENNI}
     ${CONAN_OPENCV}
     ${CONAN_BOOST}
     ${CONAN_FFMPEG}
)
