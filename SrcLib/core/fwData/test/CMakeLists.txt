fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwTest
    fwTools
    fwData
    fwComEd
)

find_package(CppUnit)

find_package(Boost COMPONENTS chrono REQUIRED)

fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(
    ${CPPUNIT_LIBRARY}
    ${Boost_CHRONO_LIBRARY}
)



