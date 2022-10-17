sight_add_target(filter_image TYPE LIBRARY FAST_DEBUG ON)

find_package(ITK QUIET COMPONENTS ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT REQUIRED)
target_link_libraries(filter_image PUBLIC ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT)

if(ITK_VERSION_MAJOR GREATER 4)
    find_package(ITK QUIET COMPONENTS ITKSmoothing ITKRegistrationMethodsv4 REQUIRED)
    target_link_libraries(filter_image PUBLIC ITKSmoothing ITKRegistrationMethodsv4)

    # Unfortunately, we cannot simply test the absence of an ITK module with find_package, so we have to hack it...
    find_file(
        HAS_ITK_REVIEW_MODULE
        NAMES ITKReview.cmake
        PATHS "${ITK_DIR}/Modules"
        NO_DEFAULT_PATH
    )
    if(HAS_ITK_REVIEW_MODULE)
        target_compile_definitions(filter_image PUBLIC "HAS_ITK_REVIEW_MODULE")
    endif()
else()
    target_compile_definitions(filter_image PUBLIC "HAS_ITK_REVIEW_MODULE")
endif()

find_package(glm QUIET REQUIRED)
if(WIN32)
    target_link_libraries(filter_image PRIVATE glm::glm)
else()
    # Hacky, no longer needed when glm-0.9.9.8+ds-3 is available
    target_include_directories(filter_image SYSTEM PRIVATE ${GLM_INCLUDE_DIRS})
endif()

target_link_libraries(filter_image PUBLIC data geometry_data io_itk)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
