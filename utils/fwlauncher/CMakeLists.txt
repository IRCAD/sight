sight_target( fwlauncher TYPE EXECUTABLE WARNINGS_AS_ERRORS ON )


# TODO: We should manage to get rid of this definition
target_compile_definitions(${FWPROJECT_NAME} PRIVATE SIGHT_MODULE_RC_PREFIX="${SIGHT_MODULE_RC_PREFIX}")

find_package(Boost QUIET COMPONENTS program_options REQUIRED)
fwLink(${Boost_PROGRAM_OPTIONS_LIBRARY})


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
)
