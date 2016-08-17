fwLoadProperties()


find_package (Boost COMPONENTS log REQUIRED)

fwLink(
    ${Boost_LOG_LIBRARY}
)
