fwLoadProperties()

find_package (Boost COMPONENTS filesystem system program_options REQUIRED)

fwInclude(
    ${Boost_INCLUDE_DIRS}
)

fwLink(
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
)
