fwLoadProperties()
find_package(CppUnit)

fwUseForwardInclude(fwMedDataTools)

fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})



