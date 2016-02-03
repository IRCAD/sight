
fwLoadProperties()

find_package (Boost COMPONENTS date_time REQUIRED)
find_package(CppUnit)

find_library(cgogn_LIBRARY NAMES cgogn libcgogn PATHS ${EXTERNAL_LIBRARIES}/lib)

fwInclude(
    ${Boost_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
    ${EXTERNAL_LIBRARIES}/include/cgogn/include
)
fwLink(
    ${Boost_DATA_TIME_LIBRARIES}
    ${cgogn_LIBRARY}
    ${CPPUNIT_LIBRARY}
)
