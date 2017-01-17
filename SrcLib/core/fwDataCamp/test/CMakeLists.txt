add_definitions(-DCAMP_COMPILATION)

fwLoadProperties()
find_package(CppUnit)
find_package(CAMP REQUIRED)

fwInclude(
    ${CAMP_INCLUDE_DIR}
    ${CPPUNIT_INCLUDE_DIR}
)
fwLink(
    ${CAMP_LIBRARIES}
    ${CPPUNIT_LIBRARY}
)
