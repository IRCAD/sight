sight_add_target(sightrun TYPE EXECUTABLE)

# TODO: We should manage to get rid of this definition
target_compile_definitions(sightrun PRIVATE SIGHT_MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")

find_package(Boost QUIET COMPONENTS program_options REQUIRED)
target_link_libraries(sightrun PRIVATE Boost::program_options)

target_link_libraries(sightrun PUBLIC core)
