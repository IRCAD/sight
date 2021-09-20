sight_add_target( geometry_data TYPE LIBRARY PCH pchData WARNINGS_AS_ERRORS OFF)

find_package(glm QUIET REQUIRED)
target_include_directories(geometry_data SYSTEM PUBLIC ${GLM_INCLUDE_DIRS})

target_link_libraries(geometry_data PUBLIC core data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
