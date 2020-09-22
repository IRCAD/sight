fwLoadProperties()

find_package(CAMP QUIET REQUIRED)

fwInclude(
    ${CAMP_INCLUDE_DIR}
)
fwLink(
    ${CAMP_LIBRARIES}
)
