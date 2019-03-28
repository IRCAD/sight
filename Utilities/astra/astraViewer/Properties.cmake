
set( NAME astraViewer )
set( VERSION 0.1 )
set( TYPE EXECUTABLE )
set( OPTIONS CONSOLE TRUE )
set( DEPENDENCIES )
set( REQUIREMENTS )

set( PLUGINS
    platforms
)

set( CONAN_DEPS
    ${CONAN_BOOST}
    ${CONAN_OPENCV}
    ${CONAN_OPENNI}
    ${CONAN_FFMPEG}
    ${CONAN_QT}
)
