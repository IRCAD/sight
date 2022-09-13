sight_add_target(geometry_data TYPE LIBRARY PCH pchData)

target_link_libraries(geometry_data PUBLIC core data glm::glm)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
