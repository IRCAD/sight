sight_add_target(io_itk TYPE LIBRARY)

find_package(JPEG QUIET REQUIRED)
find_package(ZLIB QUIET REQUIRED)

find_library(JPEG_LIBRARY libjpeg)

target_include_directories(io_itk SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(io_itk PRIVATE ${ZLIB_LIBRARIES} ${JPEG_LIBRARY})

# Since ITK is template-based, this makes sense to force the link for dependencies
target_link_libraries(
    io_itk
    PUBLIC ITKCommon
           ITKIOImageBase
           ITKIOJPEG
           ITKMetaIO
           itksys
           ITKIOJPEG2000
           ITKIONIFTI
)

target_link_libraries(io_itk PUBLIC core data io)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
