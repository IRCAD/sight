fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS calib3d aruco)
find_package(Ceres QUIET REQUIRED)
find_package(FFMPEG QUIET REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_calib3d opencv_aruco)
target_link_libraries(${FWPROJECT_NAME} PUBLIC ${CERES_LIBRARIES})
target_link_libraries(${FWPROJECT_NAME} PUBLIC ${FFMPEG_LIBRARIES})

fwInclude(
    ${CERES_INCLUDE_DIRS}
)
