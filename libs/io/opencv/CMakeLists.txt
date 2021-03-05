sight_add_target( io_opencv TYPE LIBRARY )


find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d)
target_link_libraries(io_opencv PUBLIC opencv_calib3d)

target_link_libraries(io_opencv PUBLIC 
                      core
                      data
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
