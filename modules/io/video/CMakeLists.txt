sight_target( module_io_video TYPE MODULE )


find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_videoio)

target_link_libraries(${FWPROJECT_NAME} PRIVATE opencv_videoio)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      io_base
                      ui_base
                      service
)
