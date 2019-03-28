
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
    ${CONAN_BOOST}
    ${CONAN_GDCM}
)