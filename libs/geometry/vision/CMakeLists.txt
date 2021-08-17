sight_add_target( geometry_vision TYPE LIBRARY PCH OFF )

target_link_libraries(geometry_vision PUBLIC core data service geometry_eigen)

target_link_libraries(geometry_vision PUBLIC opencv_calib3d opencv_aruco)

target_link_libraries(geometry_vision PUBLIC ${CERES_LIBRARIES})
target_include_directories(geometry_vision SYSTEM PRIVATE ${CERES_INCLUDE_DIRS})

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
