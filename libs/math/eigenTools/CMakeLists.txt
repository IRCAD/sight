fwLoadProperties()

find_package(Eigen3 QUIET REQUIRED)

target_link_libraries (${FWPROJECT_NAME} PUBLIC Eigen3::Eigen)
