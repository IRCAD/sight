sight_add_target(geometry_vision TYPE LIBRARY)

target_link_libraries(geometry_vision PUBLIC core data service geometry_eigen)

find_package(OpenCV REQUIRED COMPONENTS core)

if(OpenCV_VERSION VERSION_LESS "5.0")
    find_package(OpenCV REQUIRED COMPONENTS calib3d aruco)
    target_link_libraries(geometry_vision PUBLIC opencv_calib3d opencv_aruco)
else()
    find_package(OpenCV REQUIRED COMPONENTS calib geometry stereo objdetect)
    target_link_libraries(geometry_vision PUBLIC opencv_calib opencv_geometry opencv_stereo opencv_objdetect)
endif()

target_link_libraries(geometry_vision PRIVATE ceres)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
