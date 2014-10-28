fwLoadProperties()

fwUseForwardInclude(
    fwData
    fwDataTools
    fwGui
    fwRuntime
    fwServices

    io
)


find_package (Boost COMPONENTS system regex filesystem date_time chrono thread log log_setup REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})


