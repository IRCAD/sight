fwLoadProperties()

find_package(Boost QUIET COMPONENTS regex thread iostreams REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC Boost::regex Boost::thread  Boost::iostreams)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    #TODO write FindPsAPI ...
    target_link_libraries(${FWPROJECT_NAME} PRIVATE psapi)
endif()
