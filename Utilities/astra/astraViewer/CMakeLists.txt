fwLoadProperties()

find_package(OpenNI2 QUIET REQUIRED)
find_package(OpenCV QUIET REQUIRED highgui videoio)
find_package(FFMPEG QUIET REQUIRED)
# Qt5 is required since we use highgui
find_package(Qt5 QUIET COMPONENTS Core Gui Widgets REQUIRED)
find_package(Boost QUIET COMPONENTS program_options filesystem REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::program_options Boost::filesystem )

fwInclude(
    ${OPENNI2_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    )

fwLink(
    opencv_highgui
    opencv_videoio
    ${OPENNI2_LIBRARY}
    ${FFMPEG_LIBRARIES}
    )
