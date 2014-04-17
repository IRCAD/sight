fwLoadProperties()
fwUseForwardInclude(
    fwRuntime
    fwServices
)

find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})

