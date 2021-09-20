sight_add_target( filter_vision TYPE LIBRARY WARNINGS_AS_ERRORS OFF)


find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_ml opencv_imgproc)

target_link_libraries(filter_vision PUBLIC opencv_ml opencv_imgproc)

target_link_libraries(filter_vision PUBLIC 
                      core
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
