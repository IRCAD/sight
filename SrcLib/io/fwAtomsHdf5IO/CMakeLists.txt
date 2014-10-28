fwLoadProperties()
fwUseForwardInclude(
    fwCore
)

find_package(HDF5 REQUIRED)

find_package (Boost COMPONENTS filesystem REQUIRED)



fwForwardInclude(
    ${HDF5_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
    )
fwLink(
    ${HDF5_LIBRARIES}
    ${Boost_LIBRARIES}
    )

