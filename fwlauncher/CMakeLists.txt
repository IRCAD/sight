fwLoadProperties()

# TODO: We should manage to get rid of this definition
target_compile_definitions(${FWPROJECT_NAME} PRIVATE BUNDLE_RC_PREFIX="${BUNDLE_RC_PREFIX}")

find_package (Boost COMPONENTS program_options REQUIRED)
fwLink(${Boost_PROGRAM_OPTIONS_LIBRARY})

