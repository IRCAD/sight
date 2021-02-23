sight_target( calDataGenerator TYPE EXECUTABLE CONSOLE ON )


find_package(Boost QUIET REQUIRED COMPONENTS program_options)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::program_options)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_highgui)
target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_highgui)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      geometry_vision
)
