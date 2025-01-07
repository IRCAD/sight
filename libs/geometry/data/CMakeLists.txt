sight_add_target(geometry_data TYPE LIBRARY)

target_link_libraries(geometry_data PUBLIC core data geometry)

target_link_libraries(geometry_data PRIVATE "$<$<VERSION_GREATER_EQUAL:glm_VERSION,0.9.9.8>:glm::glm>")

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
