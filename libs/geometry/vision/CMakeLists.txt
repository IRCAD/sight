sight_add_target( geometry_vision TYPE LIBRARY PCH OFF )

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d opencv_aruco)
target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_calib3d opencv_aruco)

find_package(Qt5 QUIET COMPONENTS Test REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Test)

find_package(Ceres QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC ${CERES_LIBRARIES})

target_include_directories(${FWPROJECT_NAME} SYSTEM PRIVATE ${CERES_INCLUDE_DIRS})

target_link_libraries(${FWPROJECT_NAME} PUBLIC core data geometry_eigen)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
