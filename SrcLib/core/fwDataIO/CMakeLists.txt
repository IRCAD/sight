fwLoadProperties()
fwUseForwardInclude(
    fwCore
    fwData
    fwTools
)

#racy compatibility
add_definitions(-DPRJ_NAME=\"${NAME}\")

find_package (Boost COMPONENTS filesystem REQUIRED)
find_package( ZLIB REQUIRED )
fwInclude(${ZLIB_INCLUDE_DIRS})
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES} ${ZLIB_LIBRARIES})
