fwLoadProperties()
fwUseForwardInclude(
    fwGui
    fwRuntime
)

find_package (Boost COMPONENTS filesystem REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

