fwLoadProperties()

find_package(Boost REQUIRED)
find_package(DCMTK REQUIRED)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS}
    ${DCMTK_config_INCLUDE_DIRS}
    )
fwLink(
    ${Boost_LIBRARIES}
    ${DCMTK_LIBRARIES}
    )
