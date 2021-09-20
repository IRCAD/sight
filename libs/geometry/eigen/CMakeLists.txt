sight_add_target( geometry_eigen TYPE LIBRARY)

# find_package(Eigen3 QUIET REQUIRED) is performed in the Dependencies.cmake file

target_link_libraries (geometry_eigen PUBLIC Eigen3::Eigen)

target_link_libraries(geometry_eigen PUBLIC 
                      core
                      data
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
