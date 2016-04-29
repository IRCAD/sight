fwLoadProperties()

find_package( ZLIB REQUIRED )
fwInclude(${ZLIB_INCLUDE_DIRS}) # No forward, minizip should be private
fwLink(${ZLIB_LIBRARIES})

