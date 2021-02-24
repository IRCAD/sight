sight_add_target( module_navigation_optics TYPE MODULE )


find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_aruco)

target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_aruco)

add_dependencies(${FWPROJECT_NAME} 
                 data
                 module_service
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      service
                      geometry_data
                      io_opencv
)
