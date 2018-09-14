
set( NAME calDataGenerator )
set( VERSION 0.1 )
set( TYPE EXECUTABLE )
set( OPTIONS CONSOLE TRUE )
set( DEPENDENCIES calibration3d)
set( REQUIREMENTS  )

set( CONAN_DEPS
    boost/1.67.0@fw4spl/stable
    opencv/3.4.3@fw4spl/stable
)