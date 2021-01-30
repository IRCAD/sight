fwLoadProperties()

find_package(realsense2 QUIET REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE ${realsense2_LIBRARY})