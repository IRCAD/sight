sight_add_target(module_io_realsense TYPE MODULE FAST_DEBUG ON)

find_package(realsense2 QUIET REQUIRED)

target_include_directories(module_io_realsense SYSTEM PRIVATE ${REALSENSE2_INCLUDE_DIRS})
target_link_libraries(module_io_realsense PRIVATE ${realsense2_LIBRARY})

target_link_libraries(module_io_realsense PUBLIC core data io ui service)
