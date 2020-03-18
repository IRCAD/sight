fwLoadProperties()

find_package(realsense2 QUIET REQUIRED)

fwInclude(
    ${REALSENSE2_INCLUDE_DIRS}
)

fwLink(
    ${realsense2_LIBRARY}
)
