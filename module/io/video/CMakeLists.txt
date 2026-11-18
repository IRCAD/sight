sight_add_target(module_io_video TYPE MODULE)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_videoio)

target_link_libraries(module_io_video PRIVATE opencv_videoio)

target_link_libraries(module_io_video PUBLIC core data io ui service)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif()
