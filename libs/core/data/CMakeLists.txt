sight_add_target( data TYPE LIBRARY WARNINGS_AS_ERRORS ON )

find_package(CAMP QUIET REQUIRED)
target_include_directories(data SYSTEM PRIVATE ${CAMP_INCLUDE_DIR})
target_link_libraries(data PRIVATE ${CAMP_LIBRARIES})

target_link_libraries(data PUBLIC core )

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
