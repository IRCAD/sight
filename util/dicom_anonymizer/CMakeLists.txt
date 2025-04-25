sight_add_target(dicom_anonymizer TYPE EXECUTABLE CONSOLE ON)

find_package(Boost QUIET COMPONENTS program_options REQUIRED)
target_link_libraries(dicom_anonymizer PRIVATE Boost::program_options)

target_link_libraries(dicom_anonymizer PRIVATE core io_dicom)
