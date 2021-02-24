sight_add_target( utestDataTest TYPE TEST )

find_package(CppUnit QUIET REQUIRED)
fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})




target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      utestData
                      data
)
