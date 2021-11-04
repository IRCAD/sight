sight_add_target(
    io_itk
    TYPE LIBRARY
    PCH pchData
    WARNINGS_AS_ERRORS OFF
)

find_package(ITK QUIET REQUIRED)
find_package(JPEG QUIET REQUIRED)
find_package(ZLIB QUIET REQUIRED)

find_library(JPEG_LIBRARY libjpeg)

target_include_directories(io_itk SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_include_directories(io_itk PUBLIC ${ITK_INCLUDE_DIRS})
target_link_libraries(io_itk PRIVATE ${ZLIB_LIBRARIES} ${JPEG_LIBRARY})
# Since ITK is template-based, this makes sense to force the link for dependencies
target_link_libraries(io_itk PUBLIC ITKCommon ITKIOImageBase ITKIOJPEG ITKMetaIO itksys itkopenjpeg)

target_link_libraries(io_itk PUBLIC core data io_base)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
