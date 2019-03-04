fwLoadProperties()

find_package(OpenNI2 QUIET REQUIRED)
find_package(Boost QUIET REQUIRED COMPONENTS program_options filesystem)
find_package(OpenCV QUIET REQUIRED COMPONENTS videoio highgui)
find_package(FFMPEG QUIET REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::program_options Boost::filesystem)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_videoio opencv_highgui)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${OPENNI2_LIBRARIES})
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})

fwInclude(
    ${OPENNI2_INCLUDE_DIRS}
)
