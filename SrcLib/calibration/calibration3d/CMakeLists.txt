fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d opencv_aruco)
target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_calib3d opencv_aruco)

find_package(FFMPEG QUIET REQUIRED)
fwForwardLink(${FFMPEG_LIBRARIES})

find_package(Qt5 QUIET COMPONENTS Test REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Test)

find_package(Ceres QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC ${CERES_LIBRARIES})

fwInclude(
    ${CERES_INCLUDE_DIRS}
)
