fwLoadProperties()

fwUseForwardInclude(
    fwCore
    fwTest
    fwMath
    fwTools
)

find_package(CppUnit)
fwInclude(${CPPUNIT_INCLUDE_DIR})

fwLink(${CPPUNIT_LIBRARY})


