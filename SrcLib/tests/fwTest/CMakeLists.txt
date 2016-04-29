fwLoadProperties()

find_package(CppUnit REQUIRED)
find_package(Boost COMPONENTS chrono REQUIRED)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
)

fwLink(
    ${CPPUNIT_LIBRARY}
    ${Boost_CHRONO_LIBRARY}
)
