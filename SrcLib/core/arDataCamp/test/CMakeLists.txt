fwLoadProperties()

find_package(CppUnit)
find_package(CAMP REQUIRED)


fwInclude(
    ${CPPUNIT_INCLUDE_DIR}
    ${CAMP_INCLUDE_DIR}
)

fwLink(
    ${CPPUNIT_LIBRARY}
    ${CAMP_LIBRARIES}
)

target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DCAMP_COMPILATION")
