fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_imgcodecs)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_imgcodecs)

find_package(CppUnit QUIET REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${CPPUNIT_LIBRARIES})

find_package(glm QUIET REQUIRED)
target_include_directories(${FWPROJECT_NAME} PRIVATE ${GLM_INCLUDE_DIRS})
