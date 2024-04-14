sight_add_target(geometry_glm TYPE LIBRARY)

target_link_libraries(geometry_glm PUBLIC core)

if(WIN32)
    target_link_libraries(geometry_glm PUBLIC glm::glm)
else()
    # Hacky, no longer needed when glm-0.9.9.8+ds-3 is available
    find_package(glm QUIET REQUIRED)
    target_include_directories(geometry_glm SYSTEM PUBLIC ${GLM_INCLUDE_DIRS})
endif()

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
