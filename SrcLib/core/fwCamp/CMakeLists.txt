
fwLoadProperties()

find_package (Boost REQUIRED)
find_package (CAMP REQUIRED)

fwForwardInclude(
    ${Boost_INCLUDE_DIRS}
    ${CAMP_INCLUDE_DIR}
    )
fwLink(
    ${CAMP_LIBRARIES}
    )

