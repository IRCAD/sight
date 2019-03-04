fwLoadProperties()

find_package(glm QUIET REQUIRED)
find_package(CppUnit QUIET REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE ${CPPUNIT_LIBRARIES})

fwInclude(
    ${GLM_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
)
