fwLoadProperties()

fwQuickFindPCL()
find_package(librealsense QUIET REQUIRED)

fwForwardInclude(
    ${PCL_INCLUDE_DIRS}
    ${LIBREALSENSE_INCLUDE_DIRS}
)

fwLink(
    ${PCL_LIBRARIES}
    ${LIBREALSENSE_LIBRARIES}
)

