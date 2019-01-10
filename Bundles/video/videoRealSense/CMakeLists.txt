fwLoadProperties()

fwQuickFindPCL()
find_package(realsense2 QUIET REQUIRED)

fwInclude(
    ${PCL_INCLUDE_DIRS}
    ${REALSENSE2_INCLUDE_DIRS}
)

fwLink(
    ${PCL_LIBRARIES}
    ${realsense2_LIBRARY}
)
