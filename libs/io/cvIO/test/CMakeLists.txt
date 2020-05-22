fwLoadProperties()

find_package(Qt5 QUIET COMPONENTS Test REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Test)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_highgui)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_highgui)

find_package(FFMPEG QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})
