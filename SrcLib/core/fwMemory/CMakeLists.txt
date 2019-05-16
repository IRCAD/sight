fwLoadProperties()

find_package(Boost QUIET COMPONENTS regex filesystem thread iostreams REQUIRED)

target_link_libraries(${FWPROJECT_NAME} PUBLIC Boost::regex Boost::filesystem Boost::thread  Boost::iostreams)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    #TODO write FindPsAPI ...
    target_link_libraries(${FWPROJECT_NAME} PRIVATE psapi)
endif()
