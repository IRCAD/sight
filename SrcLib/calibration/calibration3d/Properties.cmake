set( NAME calibration3d )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwData
    eigenTools
)
set( REQUIREMENTS  )
set( DISABLE_PCH TRUE)
set( CONAN_DEPS
    ceres/1.14.0@sight/stable
    opencv/3.4.3-r3@sight/stable
    ffmpeg/4.1-r2@sight/stable
    qt/5.11.2@sight/stable
)
