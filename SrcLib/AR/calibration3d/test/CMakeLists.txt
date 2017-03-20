fwLoadProperties()

find_package(OpenCV REQUIRED)
find_package(CppUnit REQUIRED)

fwInclude(
    ${OpenCV_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
)

fwLink(
    opencv_core
    opencv_calib3d
    ${CPPUNIT_LIBRARY}
)

