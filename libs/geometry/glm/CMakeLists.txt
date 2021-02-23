sight_target( geometry_glm TYPE LIBRARY )


find_package(glm QUIET REQUIRED)

fwForwardInclude(
    ${GLM_INCLUDE_DIRS}
)
target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
)
