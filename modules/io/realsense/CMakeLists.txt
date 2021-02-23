sight_target( module_io_realsense TYPE MODULE PCH pchServicesOmp )


find_package(OpenMP QUIET)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


find_package(realsense2 QUIET REQUIRED)

fwInclude(
    ${REALSENSE2_INCLUDE_DIRS}
)

fwLink(
    ${realsense2_LIBRARY}
)


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      ui_base
                      service
)
