sight_add_target( filter_vision TYPE LIBRARY )


find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_ml opencv_imgproc)

target_link_libraries(filter_vision PUBLIC opencv_ml opencv_imgproc)

target_link_libraries(filter_vision PUBLIC 
                      core
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
