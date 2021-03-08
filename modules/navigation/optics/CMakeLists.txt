sight_add_target( module_navigation_optics TYPE MODULE )


find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_aruco)

target_link_libraries(module_navigation_optics PRIVATE opencv_aruco)

add_dependencies(module_navigation_optics 
                 data
                 module_service
)

target_link_libraries(module_navigation_optics PUBLIC 
                      core
                      data
                      service
                      geometry_data
                      io_opencv
)
