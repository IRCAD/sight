sight_add_target( activity TYPE LIBRARY PCH pchData )

target_link_libraries(${FWPROJECT_NAME} PUBLIC core data)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
