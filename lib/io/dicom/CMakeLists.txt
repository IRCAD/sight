sight_add_target(io_dicom TYPE LIBRARY)

find_package(Boost QUIET COMPONENTS locale REQUIRED)

target_link_libraries(io_dicom PRIVATE "$<$<VERSION_GREATER_EQUAL:glm_VERSION,0.9.9.8>:glm::glm>")

target_link_libraries(io_dicom PRIVATE Boost::locale PUBLIC gdcmMSFF)

target_link_libraries(
    io_dicom
    PUBLIC io
           filter_dicom
           core
           geometry
           geometry_data
           data
           service
           io_zip
           io_bitmap
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test/ut)
endif(SIGHT_BUILD_TESTS)
