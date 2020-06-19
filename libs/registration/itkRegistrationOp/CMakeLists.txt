fwLoadProperties()

find_package(ITK QUIET COMPONENTS ITKOptimizersv4 ITKTransform REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC ITKOptimizersv4 ITKTransform)

