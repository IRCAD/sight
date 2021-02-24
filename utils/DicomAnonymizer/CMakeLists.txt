sight_add_target( DicomAnonymizer TYPE EXECUTABLE CONSOLE ON )

find_package(Boost QUIET COMPONENTS program_options REQUIRED)
target_link_libraries(${FWPROJECT_NAME} PRIVATE Boost::program_options)

target_link_libraries(${FWPROJECT_NAME} PRIVATE core io_dicom)
