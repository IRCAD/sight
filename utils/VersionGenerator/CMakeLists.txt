fwLoadProperties()

find_package(Boost QUIET COMPONENTS program_options REQUIRED)

fwLink(
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
)
