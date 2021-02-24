sight_add_target( geometry_eigen TYPE LIBRARY )


find_package(Eigen3 QUIET REQUIRED)

target_link_libraries (${FWPROJECT_NAME} PUBLIC Eigen3::Eigen)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
)
