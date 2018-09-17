fwLoadProperties()

find_package(CAMP QUIET REQUIRED)
find_package(Boost QUIET COMPONENTS log log_setup filesystem REQUIRED)

fwForwardInclude(
    ${CAMP_INCLUDE_DIR}
)

fwLink(
    ${CAMP_LIBRARIES}
    ${Boost_LOG_LIBRARY}
    ${Boost_LOG_SETUP_LIBRARY}
)


target_compile_definitions(${FWPROJECT_NAME} PRIVATE "-DCAMP_COMPILATION")
