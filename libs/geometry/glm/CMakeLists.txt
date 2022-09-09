sight_add_target(geometry_glm TYPE LIBRARY)

find_package(glm QUIET REQUIRED)

target_link_libraries(geometry_glm PUBLIC core glm::glm)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
