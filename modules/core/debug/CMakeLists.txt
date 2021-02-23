fwLoadProperties()

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    #TODO write FindDbgHelp ...
    fwLink(dbghelp)
endif()


target_link_libraries(${FWPROJECT_NAME} PUBLIC 
                      core
                      data
                      ui_base
                      service
)
