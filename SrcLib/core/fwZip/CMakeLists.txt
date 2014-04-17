fwLoadProperties()
fwUseForwardInclude(
    fwCore
)


find_package (Boost COMPONENTS filesystem REQUIRED)
find_package( ZLIB REQUIRED )
fwInclude(${ZLIB_INCLUDE_DIRS}) # No forward, minizip should be private
fwForwardInclude(${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES} ${ZLIB_LIBRARIES})

