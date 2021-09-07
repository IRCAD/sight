sight_add_target( pchServiceOmp TYPE LIBRARY )

# include first to be taken into account in the PCH compile flags
find_package(OpenMP QUIET)

if (OPENMP_CXX_FOUND)
    target_compile_options(pchServiceOmp PUBLIC ${OpenMP_CXX_FLAGS})
endif()


target_link_libraries(pchServiceOmp PUBLIC service)
