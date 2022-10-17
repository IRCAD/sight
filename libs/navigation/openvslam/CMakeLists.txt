sight_add_target(navigation_openvslam TYPE LIBRARY PCH OFF)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d)
target_link_libraries(navigation_openvslam PUBLIC opencv_calib3d)

find_package(openvslam QUIET REQUIRED COMPONENTS openvslam)
target_link_libraries(navigation_openvslam PUBLIC openvslam::openvslam)

target_link_libraries(navigation_openvslam PUBLIC data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
