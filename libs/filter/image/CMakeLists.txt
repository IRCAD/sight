sight_add_target( filter_image TYPE LIBRARY PCH pchServicesOmp )

find_package(ITK QUIET COMPONENTS ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT REQUIRED)
target_link_libraries(filter_image PUBLIC ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT)

if(MSVC)
    find_package(ITK QUIET COMPONENTS ITKSmoothing ITKRegistrationMethodsv4 REQUIRED)
    target_link_libraries(filter_image PUBLIC ITKSmoothing ITKRegistrationMethodsv4)
endif()

find_package(OpenMP QUIET)
if (OPENMP_FOUND)
    target_link_libraries(filter_image PRIVATE OpenMP::OpenMP_CXX)
endif()

find_package(glm QUIET REQUIRED)
target_include_directories(filter_image SYSTEM PRIVATE ${GLM_INCLUDE_DIRS})

target_link_libraries(filter_image PUBLIC data geometry_data io_itk)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
