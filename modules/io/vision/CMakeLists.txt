sight_add_target(module_io_vision TYPE MODULE)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_core opencv_imgcodecs)
target_link_libraries(module_io_vision PRIVATE opencv_core opencv_imgcodecs)

add_dependencies(module_io_vision data module_service)

target_link_libraries(
    module_io_vision
    PUBLIC core
           data
           service
           ui_base
           io_base
           io_opencv
           geometry_vision
)
