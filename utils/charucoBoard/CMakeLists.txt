sight_add_target( charucoBoard TYPE EXECUTABLE CONSOLE ON )


find_package(Boost QUIET REQUIRED COMPONENTS program_options)
target_link_libraries(charucoBoard PRIVATE Boost::program_options)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_highgui)
target_link_libraries(charucoBoard PRIVATE opencv_highgui)

target_link_libraries(charucoBoard PUBLIC 
                      geometry_vision
)
