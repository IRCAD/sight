
set( NAME fwDicomIOFilterTest )
set( VERSION  )
set( TYPE TEST )
set( DEPENDENCIES
    fwDicomIOFilter
    fwTools
    fwCore
    fwTest
    fwMath
    fwData
    fwMedData
    fwGdcmIO
    fwLog
    )
set( REQUIREMENTS

    )
set( CONAN_DEPS
    gdcm/2.8.6@fw4spl/stable
    dcmtk/3.6.3@fw4spl/stable
)