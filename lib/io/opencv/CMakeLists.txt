sight_add_target(io_opencv TYPE LIBRARY)

find_package(OpenCV REQUIRED COMPONENTS core)

if(OpenCV_VERSION VERSION_LESS "5.0")
    find_package(OpenCV REQUIRED COMPONENTS calib3d)
    target_link_libraries(io_opencv PUBLIC opencv_calib3d)
else()
    find_package(OpenCV REQUIRED COMPONENTS geometry)
    target_link_libraries(io_opencv PUBLIC opencv_geometry)
endif()

target_link_libraries(io_opencv PUBLIC core data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
