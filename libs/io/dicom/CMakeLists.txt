sight_add_target( io_dicom TYPE LIBRARY )


find_package(Boost QUIET COMPONENTS locale REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::locale PUBLIC gdcmMSFF)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      io_base
                      filter_dicom
                      core
                      geometry_data
                      data
                      service
                      io_zip
)
