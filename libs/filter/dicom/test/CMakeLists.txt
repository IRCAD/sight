sight_add_target( filter_dicomTest TYPE TEST)

find_package(DCMTK QUIET REQUIRED)
target_link_libraries(filter_dicomTest PUBLIC
    ${DCMTK_LIBRARIES}
)

find_package(GDCM QUIET REQUIRED COMPONENTS gdcmCommon gdcmMSFF)
target_link_libraries(filter_dicomTest PUBLIC
    gdcmCommon
    gdcmMSFF
)

target_link_libraries(filter_dicomTest PUBLIC 
                      core
                      data
                      io_dicom
                      filter_dicom
                      geometry_data
                      utestData
)
