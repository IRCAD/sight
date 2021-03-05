sight_add_target( geometry_eigen TYPE LIBRARY )


find_package(Eigen3 QUIET REQUIRED)

target_link_libraries (geometry_eigen PUBLIC Eigen3::Eigen)

target_link_libraries(geometry_eigen PUBLIC 
                      core
                      data
)

if(BUILD_TESTS)
    add_subdirectory(test)
endif(BUILD_TESTS)
