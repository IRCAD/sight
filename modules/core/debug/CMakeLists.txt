sight_add_target(module_debug TYPE MODULE)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    #TODO write FindDbgHelp ...
    target_link_libraries(module_debug PRIVATE dbghelp)
endif()

target_link_libraries(module_debug PUBLIC core data ui_base service)
