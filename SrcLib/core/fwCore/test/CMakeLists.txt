fwLoadProperties()

find_package(Boost COMPONENTS chrono regex REQUIRED)
    
fwInclude(
)
fwLink(
    ${Boost_CHRONO_LIBRARY}
    ${Boost_REGEX_LIBRARY}
)
if(NOT WIN32 AND NOT APPLE)
    # missing link on Debian with system lib
    fwLink(
        pthread
    )
endif()




