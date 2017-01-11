fwLoadProperties()

find_package(CppUnit)
find_package(Boost COMPONENTS chrono REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)

fwInclude(
    ${Qt5Core_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
)
fwLink(
    ${Qt5Core_LIBRARIES}
    ${Boost_CHRONO_LIBRARY}
    ${CPPUNIT_LIBRARY}
)

