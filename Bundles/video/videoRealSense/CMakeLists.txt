
find_package(OpenMP QUIET)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

fwLoadProperties()

find_package(realsense2 QUIET REQUIRED)

fwInclude(
    ${REALSENSE2_INCLUDE_DIRS}
)

fwLink(
    ${realsense2_LIBRARY}
)

