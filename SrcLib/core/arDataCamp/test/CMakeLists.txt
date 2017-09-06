fwLoadProperties()

find_package(CAMP REQUIRED)


fwInclude(
    ${CAMP_INCLUDE_DIR}
)

fwLink(
    ${CAMP_LIBRARIES}
)

target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DCAMP_COMPILATION")
