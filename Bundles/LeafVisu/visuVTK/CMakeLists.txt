fwLoadProperties()
fwUseForwardInclude(
    fwRuntime
)


find_package(Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

fwReq(visu)
