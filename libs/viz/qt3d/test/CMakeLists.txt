sight_add_target( viz_qt3dTest TYPE TEST WARNINGS_AS_ERRORS ON )


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
