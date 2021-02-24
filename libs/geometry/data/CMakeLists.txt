sight_add_target( geometry_data TYPE LIBRARY PCH pchData )

find_package(glm QUIET REQUIRED)

target_include_directories(${FWPROJECT_NAME} SYSTEM PUBLIC ${GLM_INCLUDE_DIRS})

target_link_libraries(${FWPROJECT_NAME} PUBLIC core data)
