sight_add_target( geometry_glm TYPE LIBRARY )

find_package(glm QUIET REQUIRED)
target_include_directories(${FWPROJECT_NAME} SYSTEM PUBLIC ${GLM_INCLUDE_DIRS})

target_link_libraries(${FWPROJECT_NAME} PUBLIC core)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
