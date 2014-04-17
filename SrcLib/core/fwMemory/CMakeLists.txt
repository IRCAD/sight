
fwLoadProperties()
fwUseForwardInclude(fwCore fwCamp fwTools fwCom)

find_package (Boost COMPONENTS system regex filesystem signals iostreams REQUIRED)
find_package (CAMP)

fwForwardInclude(${Boost_INCLUDE_DIRS})

fwLink(${Boost_LIBRARIES} ${CAMP_LIBRARIES})

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    #TODO write FindPsAPI ...
    target_link_libraries(${FWPROJECT_NAME} psapi)
endif()

