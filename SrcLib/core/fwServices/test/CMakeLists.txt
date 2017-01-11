fwLoadProperties()


find_package(CppUnit)
find_package(Boost COMPONENTS chrono REQUIRED)

fwInclude(
    ${Boost_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
)

fwLink(
    ${CPPUNIT_LIBRARY}
    ${Boost_CHRONO_LIBRARY}
)
