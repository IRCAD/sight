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
    ${CONAN_CERES}
    ${CONAN_OPENCV}
    ${CONAN_FFMPEG}
    ${CONAN_QT}
)
