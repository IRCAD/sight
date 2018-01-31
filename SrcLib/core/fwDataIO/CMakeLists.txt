fwLoadProperties()

#racy compatibility
target_compile_definitions(${FWPROJECT_NAME} PRIVATE PRJ_NAME="${NAME}")

find_package(ZLIB QUIET REQUIRED )

fwInclude(${ZLIB_INCLUDE_DIRS})

fwLink(
    ${ZLIB_LIBRARIES}
)
