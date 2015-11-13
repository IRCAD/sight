

fwLoadProperties()

fwUseForwardInclude(
    fwPacsIO
    fwTools
    fwCore
    fwTest
    fwThread
    fwData
    fwMedData
    fwDicomData
    fwDcmtkIO
)

find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

find_package(CppUnit)
fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})

