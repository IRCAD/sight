fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwTest
    fwTools
    fwData
    arData
    arDataCamp
)

find_package(CppUnit)
fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})



