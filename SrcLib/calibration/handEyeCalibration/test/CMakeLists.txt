fwLoadProperties()

find_package(Ceres QUIET REQUIRED)
fwInclude(
    ${CERES_INCLUDE_DIR}
)
fwLink(
    ${CERES_LIBRARIES}
)
