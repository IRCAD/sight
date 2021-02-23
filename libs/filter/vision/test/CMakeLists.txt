sight_target( filter_visionTest TYPE TEST )


find_package(CppUnit QUIET)

fwInclude(${CPPUNIT_INCLUDE_DIR})
fwLink(${CPPUNIT_LIBRARY})

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      filter_vision
                      utestData
)
