sight_target( DicomAnonymizer TYPE EXECUTABLE CONSOLE ON )


find_package(Boost QUIET COMPONENTS program_options REQUIRED)
fwLink(${Boost_LIBRARIES})

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      io_dicom
)
