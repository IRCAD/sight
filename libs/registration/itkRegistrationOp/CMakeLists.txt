fwLoadProperties()

if(VCPKG_TARGET_TRIPLET)
    find_package(ITK QUIET COMPONENTS ITKRegistrationMethodsv4 ITKOptimizersv4 ITKTransform ITKSmoothing REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PUBLIC ITKRegistrationMethodsv4 ITKOptimizersv4 ITKTransform ITKSmoothing)
else()
    find_package(ITK QUIET COMPONENTS ITKOptimizersv4 ITKTransform REQUIRED)
    target_link_libraries(${FWPROJECT_NAME} PUBLIC ITKOptimizersv4 ITKTransform)
endif()

