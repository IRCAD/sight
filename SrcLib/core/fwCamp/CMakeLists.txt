fwLoadProperties()
find_package(CAMP REQUIRED)

fwForwardInclude(
    ${CAMP_INCLUDE_DIR}
)

fwForwardLink(${CAMP_LIBRARIES})

