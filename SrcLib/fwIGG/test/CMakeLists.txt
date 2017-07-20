
fwLoadProperties()

find_package(Boost COMPONENTS regex REQUIRED)
find_package(ZLIB REQUIRED )
find_library(cgogn_LIBRARY NAMES cgogn libcgogn PATHS ${EXTERNAL_LIBRARIES}/lib)

fwInclude(
    ${Boost_INCLUDE_DIRS}
    ${EXTERNAL_LIBRARIES}/include/cgogn/include
    ${ZLIB_INCLUDE_DIRS}
)
fwLink(
    ${Boost_REGEX_LIBRARY}
    ${cgogn_LIBRARY}
    ${ZLIB_LIBRARIES}
)
