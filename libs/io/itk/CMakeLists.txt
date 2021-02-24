sight_add_target( io_itk TYPE LIBRARY PCH pchData )

find_package(ITK QUIET REQUIRED)
find_package(JPEG QUIET REQUIRED)
find_package(ZLIB QUIET REQUIRED )

find_library(JPEG_LIBRARY libjpeg)

target_include_directories(${FWPROJECT_NAME} SYSTEM PRIVATE ${ZLIB_INCLUDE_DIRS})
target_include_directories(${FWPROJECT_NAME} PUBLIC ${ITK_INCLUDE_DIRS})
target_link_libraries(${FWPROJECT_NAME} PRIVATE ${ZLIB_LIBRARIES} ${JPEG_LIBRARY})
# Since ITK is template-based, this makes sense to force the link for dependencies
target_link_libraries(${FWPROJECT_NAME} PUBLIC ITKCommon ITKIOImageBase ITKIOJPEG ITKMetaIO itksys itkopenjpeg)

target_link_libraries(${FWPROJECT_NAME} PUBLIC core data io_base)
