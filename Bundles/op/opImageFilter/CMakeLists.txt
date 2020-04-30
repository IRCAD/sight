fwLoadProperties()

find_package(OpenMP QUIET)

if (OPENMP_FOUND)
    target_compile_options(${FWPROJECT_NAME} PUBLIC ${OpenMP_CXX_FLAGS})
endif()

find_package(glm QUIET REQUIRED)

fwInclude(
    ${GLM_INCLUDE_DIRS}
)
