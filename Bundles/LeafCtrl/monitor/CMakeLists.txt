fwLoadProperties()
fwUseForwardInclude(
    fwGui
    fwRuntime
)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    #TODO write FindDbgHelp ...
    fwLink(dbghelp)
endif()

