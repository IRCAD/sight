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
           itkopenjpeg
           ITKIONIFTI
)

target_link_libraries(io_itk PUBLIC core data io_base)

# This is a hack to allow both Clang and gcc support
# @todo: remove this when ITK >= 5.3 is released
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND ITK_VERSION_MAJOR GREATER 4 AND ITK_VERSION_MINOR LESS 3)
    target_compile_options(
        io_itk PUBLIC $<BUILD_INTERFACE:-include ${CMAKE_CURRENT_SOURCE_DIR}/patched_itk_compiler_detection.h>
                      $<INSTALL_INTERFACE:-include io/itk/patched_itk_compiler_detection.h>
    )
endif()

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
