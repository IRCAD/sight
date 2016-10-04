
fwLoadProperties()

find_package(Boost COMPONENTS regex thread system date_time chrono REQUIRED)
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
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_THREAD_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_CHRONO_LIBRARY}
    ${cgogn_LIBRARY}
    ${CPPUNIT_LIBRARY}
    ${ZLIB_LIBRARIES}
)
