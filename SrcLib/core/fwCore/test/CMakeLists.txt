fwLoadProperties()

find_package(Boost QUIET COMPONENTS regex REQUIRED)

fwInclude(
)
fwLink(
    ${Boost_REGEX_LIBRARY}
)
if(NOT WIN32 AND NOT APPLE)
    # missing link on Debian with system lib
    fwLink(
        pthread
    )
endif()
