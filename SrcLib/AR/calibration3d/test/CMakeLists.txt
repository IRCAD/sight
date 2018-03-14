fwLoadProperties()

find_package(OpenCV QUIET REQUIRED)
find_package(CppUnit QUIET REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
)

fwLink(
    opencv_core
    opencv_calib3d
    ${CPPUNIT_LIBRARY}
)

