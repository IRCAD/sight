sight_add_target( geometry_visionTest TYPE TEST )

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_imgcodecs opencv_highgui)
target_link_libraries(${FWPROJECT_NAME} PUBLIC opencv_highgui PRIVATE opencv_imgcodecs)

find_package(glm QUIET REQUIRED)
target_include_directories(${FWPROJECT_NAME} PRIVATE ${GLM_INCLUDE_DIRS})

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      utestData
                      geometry_data
                      geometry_vision
)
