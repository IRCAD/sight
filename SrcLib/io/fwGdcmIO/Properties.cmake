
set( NAME fwGdcmIO )
set( VERSION 0.1 )
set( TYPE LIBRARY )
set( DEPENDENCIES
    fwCore
    fwData
    fwMath
    fwMedData
    fwMedDataTools
    fwDataIO
    fwTools
    fwRuntime
    fwDicomIOFilter
    fwMedDataTools
    fwLog
    fwJobs
    fwCom
    fwZip
    fwDicomTools
    fwDataTools
    fwServices
    fwDicomTools
    )
set( REQUIREMENTS )
set( CONAN_DEPS
    boost/1.67.0@fw4spl/stable
    gdcm/2.8.6@fw4spl/stable
)