fwLoadProperties()
fwUseForwardInclude(
    fwGui
    fwRuntime
)

find_package (Boost REQUIRED)
fwForwardInclude(${Boost_INCLUDE_DIRS})


if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    #TODO write FindDbgHelp ...
    target_link_libraries(${FWPROJECT_NAME} dbghelp)
endif()

