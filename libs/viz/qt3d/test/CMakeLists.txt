fwLoadProperties()

find_package(CppUnit QUIET REQUIRED)

fwForwardInclude(
    ${CPPUNIT_INCLUDE_DIR}
)

fwForwardLink(
    ${CPPUNIT_LIBRARY}
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      viz_qt3d
                      utestData
)
