
set( NAME handEyeCalibration )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwData
    fwDataCamp
    fwDataTools
    fwMath
    fwMedData
    fwTools
    eigenTools
)
set( REQUIREMENTS  )

set( CONAN_DEPS
    eigen/3.3.4@sight/stable
    ceres/1.14.0@sight/stable
    opencv/3.4.3-r1@sight/stable
)