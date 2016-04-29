fwLoadProperties()

find_package(CppUnit)
find_package(Boost COMPONENTS chrono regex REQUIRED)
    
fwInclude(
    ${CPPUNIT_INCLUDE_DIR}
)
fwLink(
    ${CPPUNIT_LIBRARY}
    ${Boost_CHRONO_LIBRARY}
    ${Boost_REGEX_LIBRARY}
)
if(NOT WIN32 AND NOT APPLE)
    # missing link on Debian with system lib
    fwLink(
        pthread
    )
endif()




