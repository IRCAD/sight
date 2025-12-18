sight_add_target(module_navigation_optics TYPE MODULE)

find_package(OpenCV QUIET REQUIRED COMPONENTS opencv_aruco)

target_link_libraries(${SIGHT_TARGET} PRIVATE opencv_aruco)

add_dependencies(${SIGHT_TARGET} data module_service)

target_link_libraries(${SIGHT_TARGET} PUBLIC core data service ui geometry_data io_opencv)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
