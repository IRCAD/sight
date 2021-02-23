sight_target( VersionGenerator TYPE EXECUTABLE CONSOLE ON )


find_package(Boost QUIET COMPONENTS program_options REQUIRED)

fwLink(
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      io_atoms
)
