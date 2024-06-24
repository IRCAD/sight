sight_add_target(geometry_data TYPE LIBRARY)

target_link_libraries(geometry_data PUBLIC core data)

if(WIN32)
    target_link_libraries(geometry_data PUBLIC glm::glm)
else()
    # Hacky, no longer needed when glm-0.9.9.8+ds-3 is available
    target_include_directories(geometry_data SYSTEM PUBLIC ${GLM_INCLUDE_DIRS})
endif()

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
