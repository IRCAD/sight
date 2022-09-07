sight_add_target(geometry_vision TYPE LIBRARY PCH OFF)

target_link_libraries(geometry_vision PUBLIC core data service geometry_eigen)

target_link_libraries(geometry_vision PUBLIC opencv_calib3d opencv_aruco)

target_link_libraries(geometry_vision PRIVATE ceres)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
