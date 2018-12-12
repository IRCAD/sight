fwLoadProperties()

find_package(OpenCV QUIET REQUIRED)
find_package(CppUnit QUIET REQUIRED)
find_package(glm QUIET REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
    ${GLM_INCLUDE_DIRS}
)

fwLink(
    opencv_core
    opencv_calib3d
    opencv_aruco
    ${CPPUNIT_LIBRARY}
)

