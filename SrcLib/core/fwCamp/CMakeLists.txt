fwLoadProperties()
find_package(CAMP QUIET REQUIRED)

fwForwardInclude(
    ${CAMP_INCLUDE_DIR}
)

fwForwardLink(${CAMP_LIBRARIES})

