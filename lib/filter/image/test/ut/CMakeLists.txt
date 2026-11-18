sight_add_target(filter_image_ut TYPE TEST)

target_link_libraries(${SIGHT_TARGET} PUBLIC utest_data data filter_image io_itk)

find_package(ITK QUIET COMPONENTS ITKCommon ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT ITKStatistics REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE ITKCommon ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT ITKStatistics)

if(ITK_VERSION_MAJOR GREATER 4)
    find_package(ITK QUIET COMPONENTS ITKSmoothing ITKRegistrationMethodsv4 REQUIRED)
    target_link_libraries(${SIGHT_TARGET} PRIVATE ITKSmoothing ITKRegistrationMethodsv4)
endif()
