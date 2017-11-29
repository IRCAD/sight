# include first to be taken into account in the PCH compile flags
include(FindOpenMP)

if (OPENMP_CXX_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
else()
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 3.6)
        message("Try forcing OpenMP support for Clang")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} -fopenmp=libiomp5")
    endif()
endif()

fwLoadProperties()
