sight_add_target( io_opencv TYPE LIBRARY WARNINGS_AS_ERRORS OFF)

target_link_libraries(io_opencv PUBLIC opencv_calib3d)

target_link_libraries(io_opencv PUBLIC core data)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
