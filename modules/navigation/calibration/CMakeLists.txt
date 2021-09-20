sight_add_target( module_navigation_calibration TYPE MODULE)


find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_calib3d)
find_package(Eigen3 QUIET REQUIRED)

target_link_libraries(module_navigation_calibration PRIVATE Eigen3::Eigen)
target_link_libraries(module_navigation_calibration PRIVATE opencv_calib3d)

target_link_libraries(module_navigation_calibration PUBLIC
                      core
                      data
                      service
                      geometry_data
                      geometry_eigen
                      geometry_vision
)
