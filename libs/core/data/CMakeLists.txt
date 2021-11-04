sight_add_target(data TYPE LIBRARY)

find_package(CAMP QUIET REQUIRED)
target_include_directories(data SYSTEM PRIVATE ${CAMP_INCLUDE_DIR})
target_link_libraries(data PRIVATE ${CAMP_LIBRARIES})

target_link_libraries(data PUBLIC core)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
