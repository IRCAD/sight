fwLoadProperties()


find_package( ZLIB REQUIRED )
fwInclude(${ZLIB_INCLUDE_DIRS})
find_library(cgogn_LIBRARY NAMES cgogn libcgogn PATHS ${EXTERNAL_LIBRARIES}/lib)

fwForwardInclude(
    ${EXTERNAL_LIBRARIES}/include/cgogn/include
    )
fwLink(
    ${cgogn_LIBRARY}
    ${ZLIB_LIBRARIES}
    )
