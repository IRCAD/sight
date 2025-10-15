sight_add_target(filter_image_ut TYPE DOCTEST)

target_link_libraries(${SIGHT_TARGET} PUBLIC utest_data data filter_image io_itk)

find_package(ITK QUIET COMPONENTS ITKCommon ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT ITKStatistics REQUIRED)
target_link_libraries(${SIGHT_TARGET} PRIVATE ITKCommon ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT ITKStatistics)

if(ITK_VERSION_MAJOR GREATER 4)
    find_package(ITK QUIET COMPONENTS ITKSmoothing ITKRegistrationMethodsv4 REQUIRED)
    target_link_libraries(${SIGHT_TARGET} PRIVATE ITKSmoothing ITKRegistrationMethodsv4)

    # Unfortunately, we cannot simply test the absence of an ITK module with find_package, so we have to hack it...
    find_file(
        HAS_ITK_REVIEW_MODULE
        NAMES ITKReview.cmake
        PATHS "${ITK_DIR}/Modules"
        NO_DEFAULT_PATH
    )

    if(HAS_ITK_REVIEW_MODULE)
        target_compile_definitions(${SIGHT_TARGET} PUBLIC "HAS_ITK_REVIEW_MODULE")
    endif()
else()
    target_compile_definitions(${SIGHT_TARGET} PUBLIC "HAS_ITK_REVIEW_MODULE")
endif()
