sight_target( io_base TYPE LIBRARY PCH pchData WARNINGS_AS_ERRORS ON )


#racy compatibility
target_compile_definitions(${FWPROJECT_NAME} PRIVATE PRJ_NAME="${NAME}")

find_package(ZLIB QUIET REQUIRED )

fwInclude(${ZLIB_INCLUDE_DIRS})

fwLink(
    ${ZLIB_LIBRARIES}
)

target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      service
)
