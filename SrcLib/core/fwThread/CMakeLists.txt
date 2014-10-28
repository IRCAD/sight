
fwLoadProperties()

find_package (Boost COMPONENTS thread REQUIRED)

fwForwardInclude(${fwCore_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
fwLink(${Boost_LIBRARIES})

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DWIN32_LEAN_AND_MEAN)
endif()

