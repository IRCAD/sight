fwLoadProperties()

find_package(ZLIB QUIET REQUIRED )
find_package(Eigen QUIET REQUIRED)

find_library(cgogn_LIBRARY NAMES cgogn libcgogn PATHS ${EXTERNAL_LIBRARIES}/lib)

fwForwardInclude(
    ${EIGEN3_INCLUDE_DIR}
    ${EXTERNAL_LIBRARIES}/include/cgogn/include
    )
fwLink(
    ${cgogn_LIBRARY}
    ${ZLIB_LIBRARIES}
    )
