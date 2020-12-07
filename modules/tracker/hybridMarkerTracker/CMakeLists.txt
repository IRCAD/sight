fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_highgui)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_highgui)

find_package(Ippe QUIET REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE
        ${PATTERN_TRACKER_LIBRARY}
        ${CHESS_DETECTOR_LIBRARY}
        ${IPPE_LIBRARIES}
        )

target_include_directories(${FWPROJECT_NAME} PRIVATE
        ${IPPE_INCLUDE_DIRS}
        ${CONAN_HYBRID_MARKER_TRACKER_ROOT}/include
        )