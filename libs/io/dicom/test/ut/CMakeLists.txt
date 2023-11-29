sight_add_target(io_dicom_ut TYPE TEST)

find_package(GDCM QUIET REQUIRED COMPONENTS gdcmCommon gdcmMSFF)
target_link_libraries(io_dicom_ut PUBLIC gdcmCommon gdcmMSFF)

target_link_libraries(io_dicom_ut PUBLIC io_dicom core data utest_data io_zip io)
