
fwLoadProperties()

find_package (Boost COMPONENTS chrono date_time REQUIRED)
find_package(ZLIB REQUIRED )
find_package(CppUnit)

find_library(cgogn_LIBRARY NAMES cgogn libcgogn PATHS ${EXTERNAL_LIBRARIES}/lib)

fwInclude(
    ${Boost_CHRONO_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${CPPUNIT_INCLUDE_DIR}
    ${EXTERNAL_LIBRARIES}/include/cgogn/include
    ${ZLIB_INCLUDE_DIRS}
)
fwLink(
    ${Boost_CHRONO_LIBRARY}
    ${Boost_DATA_TIME_LIBRARIES}
    ${cgogn_LIBRARY}
    ${CPPUNIT_LIBRARY}
    ${ZLIB_LIBRARIES}
)
