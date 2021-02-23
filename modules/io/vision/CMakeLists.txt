fwLoadProperties()

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_core opencv_imgcodecs)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_core opencv_imgcodecs)

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      service
                      ui_base
                      io_base
                      io_opencv
                      geometry_vision
)
