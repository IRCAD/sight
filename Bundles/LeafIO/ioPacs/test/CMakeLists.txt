fwLoadProperties()

# fwUseForwardInclude(
#     fwCore
#     fwTools
#     fwData
#     fwMedData
#     fwPacsIO
#     fwTest
# )

find_package(CppUnit)
fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})
