
fwLoadProperties()

find_package(Boost COMPONENTS regex REQUIRED)
find_package(ZLIB REQUIRED )
find_package(CppUnit)
find_library(cgogn_LIBRARY NAMES cgogn libcgogn PATHS ${EXTERNAL_LIBRARIES}/lib)

fwInclude(
    ${Boost_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
    ${EXTERNAL_LIBRARIES}/include/cgogn/include
    ${ZLIB_INCLUDE_DIRS}
)
fwLink(
    ${Boost_REGEX_LIBRARY}
    ${cgogn_LIBRARY}
    ${CPPUNIT_LIBRARY}
    ${ZLIB_LIBRARIES}
)
