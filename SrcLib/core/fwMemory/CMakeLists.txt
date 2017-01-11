fwLoadProperties()

find_package (Boost COMPONENTS regex signals iostreams REQUIRED)

fwLink(${Boost_LIBRARIES})

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    #TODO write FindPsAPI ...
    target_link_libraries(${FWPROJECT_NAME} PRIVATE psapi)
endif()
