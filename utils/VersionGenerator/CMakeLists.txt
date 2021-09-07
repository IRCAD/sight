sight_add_target( VersionGenerator TYPE EXECUTABLE CONSOLE ON )

find_package(Boost QUIET COMPONENTS program_options REQUIRED)
target_link_libraries(VersionGenerator PRIVATE Boost::program_options)

target_link_libraries(VersionGenerator PUBLIC io_atoms)
