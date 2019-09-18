
set( NAME styleQml )
set( VERSION 0.1 )
set( TYPE BUNDLE )
set( DEPENDENCIES
    fwRuntime
    fwQml
)
set( REQUIREMENTS )
set( PLUGINS
    QtQml
    QtQuick
    QtQuick.2
    platforms
    imageformats
)
set( CONAN_DEPS
    ${CONAN_QT}
)
