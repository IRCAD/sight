sight_add_target(io_dicom TYPE LIBRARY)

find_package(Boost QUIET COMPONENTS locale REQUIRED)

find_package(glm QUIET REQUIRED)

if(WIN32)
    target_link_libraries(io_dicom PRIVATE glm::glm)
else()
    # Hacky, no longer needed when glm-0.9.9.8+ds-3 is available
    target_include_directories(io_dicom SYSTEM PRIVATE ${GLM_INCLUDE_DIRS})
endif()

target_link_libraries(io_dicom PRIVATE Boost::locale PUBLIC gdcmMSFF)

target_link_libraries(
    io_dicom
    PUBLIC io
           filter_dicom
           core
           geometry_data
           data
           service
           io_zip
           io_bitmap
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
