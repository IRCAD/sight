fwLoadProperties()

find_package(CppUnit)
find_package(CAMP REQUIRED)

fwUseForwardInclude(
    fwCore
    fwTest
    fwTools
    fwDataCamp 
    arData
    arDataCamp
    fwData
)

fwInclude(
    ${CPPUNIT_INCLUDE_DIR}
    ${CAMP_INCLUDE_DIR}
)

fwLink(
    ${CPPUNIT_LIBRARY}
    ${CAMP_LIBRARIES}
)




