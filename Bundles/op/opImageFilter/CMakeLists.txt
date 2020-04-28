fwLoadProperties()

find_package(OpenMP QUIET)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

find_package(glm QUIET REQUIRED)

fwInclude(
    ${GLM_INCLUDE_DIRS}
)
