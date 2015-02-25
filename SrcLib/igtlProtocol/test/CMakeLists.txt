fwLoadProperties()

fwUseForwardInclude(
        fwTools
        fwData
        fwDataCamp
        igtlProtocol
)

find_package(CppUnit)

fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})



