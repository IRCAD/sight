fwLoadProperties()

fwUseForwardInclude(
    fwLog
    fwTools
    fwCore
    fwTest
)

find_package(CppUnit)
fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})

