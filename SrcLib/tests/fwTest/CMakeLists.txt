fwLoadProperties()
fwUseForwardInclude(
    fwComEd
    fwCore
    fwData
    fwDataTools
    fwMath
    fwTools
)


find_package (Boost COMPONENTS thread filesystem REQUIRED)
find_package(CppUnit REQUIRED)

fwForwardInclude(${Boost_INCLUDE_DIRS} ${CPPUNIT_INCLUDE_DIR})
fwLink(${Boost_LIBRARIES} ${CPPUNIT_LIBRARY})
