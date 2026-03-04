sight_add_target(io_dicom_ut TYPE DOCTEST)

find_package(GDCM QUIET REQUIRED COMPONENTS gdcmCommon gdcmMSFF)
target_link_libraries(io_dicom_ut PRIVATE gdcmCommon gdcmMSFF)

target_link_libraries(io_dicom_ut PRIVATE "$<$<VERSION_GREATER_EQUAL:glm_VERSION,0.9.9.8>:glm::glm>")

target_link_libraries(io_dicom_ut PRIVATE io_dicom core data utest_data io_zip io)
