sight_target( filter_image TYPE LIBRARY PCH pchServicesOmp )


find_package(ITK QUIET COMPONENTS ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PUBLIC ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT)

find_package(OpenMP QUIET)
if (OPENMP_FOUND)
    target_link_libraries(${FWPROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif()

find_package(glm QUIET REQUIRED)
fwInclude(
    ${GLM_INCLUDE_DIRS}
)


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      data
                      geometry_data
                      io_itk
)
