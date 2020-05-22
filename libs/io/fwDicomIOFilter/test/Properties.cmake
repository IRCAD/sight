
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
    ${CONAN_GDCM}
    ${CONAN_DCMTK}
)