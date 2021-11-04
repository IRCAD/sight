sight_add_target(DicomAnonymizer TYPE EXECUTABLE CONSOLE ON)

find_package(Boost QUIET COMPONENTS program_options REQUIRED)
target_link_libraries(DicomAnonymizer PRIVATE Boost::program_options)

target_link_libraries(DicomAnonymizer PRIVATE core io_dicom)
