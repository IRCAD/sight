sight_add_target(io_dicomTest TYPE TEST)

find_package(GDCM QUIET REQUIRED COMPONENTS gdcmCommon gdcmMSFF)
target_link_libraries(io_dicomTest PUBLIC gdcmCommon gdcmMSFF)

target_link_libraries(io_dicomTest PUBLIC io_dicom core data utest_data io_zip io)
