fwLoadProperties()

find_package(ITK QUIET COMPONENTS ITKRegistrationMethodsv4 ITKOptimizersv4 ITKTransform ITKSmoothing REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC ITKRegistrationMethodsv4 ITKOptimizersv4 ITKTransform ITKSmoothing)

