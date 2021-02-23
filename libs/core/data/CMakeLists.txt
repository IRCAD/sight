sight_target( data TYPE LIBRARY WARNINGS_AS_ERRORS ON )


find_package(CAMP QUIET REQUIRED)

fwInclude(
    ${CAMP_INCLUDE_DIR}
)
fwLink(
    ${CAMP_LIBRARIES}
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
)
