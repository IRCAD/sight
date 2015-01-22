fwLoadProperties()
fwUseForwardInclude(
    fwCore
)

find_package(HDF5 REQUIRED)

fwForwardInclude(
    ${HDF5_INCLUDE_DIR}
    )
fwLink(
    ${HDF5_LIBRARIES}
    )

