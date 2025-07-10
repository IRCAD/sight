sight_add_target(geometry TYPE LIBRARY)

target_link_libraries(geometry PUBLIC core)

target_link_libraries(geometry PRIVATE "$<$<VERSION_GREATER_EQUAL:glm_VERSION,0.9.9.8>:glm::glm>")

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
