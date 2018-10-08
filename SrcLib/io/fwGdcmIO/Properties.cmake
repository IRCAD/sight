
set( NAME fwGdcmIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwDataTools
    fwDataIO
    fwDicomIOFilter
    fwDicomTools
    fwJobs
    fwMath
    fwMedDataTools
    fwServices
    fwZip
    )
set( REQUIREMENTS )
set( CONAN_DEPS
    boost/1.67.0@sight/stable
    gdcm/2.8.6@sight/stable
)