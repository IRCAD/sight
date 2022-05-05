# This needs to be done before declaring the target to be defined in the PCH (MSVC)
find_package(OpenMP QUIET)

if(OPENMP_CXX_FOUND)
    add_compile_options(${OpenMP_CXX_FLAGS})
endif()

sight_add_target(pchServiceOmp TYPE LIBRARY FAST_DEBUG ON)

target_link_libraries(pchServiceOmp PUBLIC service)
