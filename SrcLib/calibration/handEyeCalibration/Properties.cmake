
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
    eigen/3.3.4@fw4spl/stable
    ceres/1.14.0@fw4spl/stable
    opencv/3.4.3@fw4spl/stable
)