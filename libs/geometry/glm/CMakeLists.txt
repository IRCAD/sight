sight_add_target( geometry_glm TYPE LIBRARY )

find_package(glm QUIET REQUIRED)
target_include_directories(geometry_glm SYSTEM PUBLIC ${GLM_INCLUDE_DIRS})

target_link_libraries(geometry_glm PUBLIC core)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
