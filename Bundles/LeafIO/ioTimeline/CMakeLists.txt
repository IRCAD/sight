fwLoadProperties()

find_package(Boost COMPONENTS filesystem REQUIRED)

fwInclude(

)

fwLink(
    ${Boost_FILESYSTEM_LIBRARY}
)

