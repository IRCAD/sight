fwLoadProperties()

find_package(ITK QUIET COMPONENTS ITKLabelMap ITKOptimizersv4 ITKTransform REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC ITKLabelMap ITKOptimizersv4 ITKTransform)

find_package(OpenMP QUIET)
if (OPENMP_FOUND)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif()

find_package(glm QUIET REQUIRED)
fwInclude(
    ${GLM_INCLUDE_DIRS}
)

