sight_add_target(filter_image TYPE LIBRARY FAST_DEBUG ON)

# On windows for a strange reason the VXL "core" directory (needed by itkArray.h) is not exported by ITK
# Mark it as a private system directory to avoid putting hardcoded paths in .cmake files
if(MSVC)
    find_package(VXL QUIET REQUIRED)
    target_include_directories(filter_image SYSTEM PRIVATE ${VXL_CORE_INCLUDE_DIRS})
endif()

find_package(ITK QUIET COMPONENTS ITKCommon ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT ITKStatistics REQUIRED)
target_link_libraries(filter_image PUBLIC ITKCommon ITKLabelMap ITKOptimizersv4 ITKTransform ITKFFT ITKStatistics)

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

if(ITK_VERSION_MAJOR GREATER 5 OR (ITK_VERSION_MAJOR EQUAL 5 AND ITK_VERSION_MINOR GREATER 3))
    # This is required to register all needed ITK factories
    include(${ITK_USE_FILE})
endif()

target_link_libraries(filter_image PRIVATE "$<$<VERSION_GREATER_EQUAL:glm_VERSION,0.9.9.8>:glm::glm>")

target_link_libraries(filter_image PUBLIC data geometry_data io_itk)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
