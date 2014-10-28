fwLoadProperties()
fwUseForwardInclude(
    fwRuntime

    io
)

find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

