sight_add_target( geometry_visionTest TYPE TEST )


find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_imgcodecs)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_imgcodecs)

find_package(glm QUIET REQUIRED)
target_include_directories(${FWPROJECT_NAME} PRIVATE ${GLM_INCLUDE_DIRS})

fwInclude(
    ${GLM_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_highgui)
target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_highgui)
target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      utestData
                      geometry_data
                      geometry_vision
)
