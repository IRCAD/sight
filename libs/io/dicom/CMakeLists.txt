sight_add_target( io_dicom TYPE LIBRARY )


find_package(Boost QUIET COMPONENTS locale REQUIRED)

target_link_libraries(io_dicom PRIVATE Boost::locale PUBLIC gdcmMSFF)

target_link_libraries(io_dicom PUBLIC 
                      io_base
                      filter_dicom
                      core
                      geometry_data
                      data
                      service
                      io_zip
)

if(SIGHT_BUILD_TESTS)
    add_subdirectory(test)
endif(SIGHT_BUILD_TESTS)
