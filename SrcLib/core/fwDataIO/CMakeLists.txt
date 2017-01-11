fwLoadProperties()


#racy compatibility
add_definitions(-DPRJ_NAME=\"${NAME}\")

find_package(ZLIB REQUIRED )

fwInclude(${ZLIB_INCLUDE_DIRS})

fwLink(
    ${ZLIB_LIBRARIES}
)
