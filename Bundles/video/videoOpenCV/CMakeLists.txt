fwLoadProperties()

find_package(Boost QUIET REQUIRED COMPONENTS filesystem)
find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_videoio)
find_package(FFMPEG QUIET REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::filesystem)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_videoio)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})
