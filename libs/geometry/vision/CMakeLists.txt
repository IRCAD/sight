sight_add_target( geometry_vision TYPE LIBRARY PCH OFF )

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d opencv_aruco)
target_link_libraries(geometry_vision PUBLIC opencv_calib3d opencv_aruco)

find_package(Qt5 QUIET COMPONENTS Test REQUIRED)
target_link_libraries(geometry_vision PRIVATE Qt5::Test)

find_package(Ceres QUIET REQUIRED)
target_link_libraries(geometry_vision PUBLIC ${CERES_LIBRARIES})

target_include_directories(geometry_vision SYSTEM PRIVATE ${CERES_INCLUDE_DIRS})

target_link_libraries(geometry_vision PUBLIC core data geometry_eigen)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
