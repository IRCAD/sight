
set( NAME astraViewer )
set( VERSION 0.1 )
set( TYPE EXECUTABLE )
set( OPTIONS CONSOLE TRUE )
set( DEPENDENCIES )
set( REQUIREMENTS  )

set( PLUGINS
    platforms
)

set( CONAN_DEPS
    qt/5.11.2@sight/stable
    boost/1.67.0@sight/stable
    opencv/3.4.3-r1@sight/stable
    openni/2.2.0-rev-958951f@sight/stable
)
