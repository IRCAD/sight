fwLoadProperties()

find_package(Boost QUIET REQUIRED COMPONENTS program_options)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::program_options)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_aruco)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_aruco)

find_package(FFMPEG QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${FFMPEG_LIBRARIES})

find_package(Qt5 QUIET COMPONENTS Test REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Test)
